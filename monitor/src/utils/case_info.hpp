#ifndef CASE_INFO_HPP
#define CASE_INFO_HPP
#include <stdexcept>
#include <unistd.h>
namespace case_info {
    typedef const unsigned char status_t;
    typedef const unsigned char op_type_t;

    namespace fuzzer_status{
        // fuzzer运行状态
        status_t RUNNING = 1;        //正在运行: 当fuzzer检测到status为此状态时，会继续进行fuzz操作
        status_t PAUSE = 2;          //暂停：当fuzzer监测到此状态时，会立刻停下fuzz
        status_t READY = 3;          //准备：当fuzzer监测到此状态时，会将status置为RUNNING，并继续运行
        status_t INTERRUPT = 4;      //中断：当fuzzer监测到此状态时，会停下手中的工作，查看op要做什么操作，然后进行相应处理后将status置为TASK_FINISHED
        status_t TASK_FINISHED = 5;  //任务完成：当服务端监测到此状态时，就知道fuzzer已经将任务完成了，
                                                //之后可以将status置为READY，让fuzzer继续运行，
                                                //也可以将status置为INTERRUPT，让fuzzer执行下一个任务
    }

    namespace op_type{
        op_type_t REFRESH_QUEUE = 1;      //更新队列里种子的信息
        op_type_t REARRANGE_QUEUE = 2;    //重排种子顺序
        op_type_t PAUSE_FUZZER = 3;       //暂停fuzz
        op_type_t RESUME_FUZZER = 4;      //恢复fuzz
    }

    const int MAX_QUEUE_LEN = 3000;         //队列最大长度
    const int MAX_CASE_FNAME_LEN = 512;     //最长文件名

    
    const int MAX_TRY_TIMES = 100;
    const int SLEEP_INTERVAL_US = 100;         //100us = 0.1ms

    struct queue_entry{
        // 种子文件名
        char fname[MAX_CASE_FNAME_LEN];
        // 种子长度
        int len;

        // 校准是否失败（Calibration failed?）
        unsigned char cal_filed;
        // 是否经过剪枝
        unsigned char trim_done;
        // 是否经过fuzz
        unsigned char was_fuzzed;
        // 是否通过重要步骤（Deterministic stage passed?）
        unsigned char passed_det;
        // 是否产生新的覆盖
        unsigned char has_new_cov;
        // 是否是感兴趣的种子
        unsigned char favored;
        // 是否被文件系统标注为冗余的
        unsigned char fs_redundant;

        // 种子执行时间(us)
        unsigned long long int exec_us;
        // 落后的队列周期数(Number of queue cycles behind)
        unsigned long long int handicap;
        // 路径深度
        unsigned long long int depth;

        // aflgo计算得出的语法距离
        double distance;
    };


    struct CaseInfo {
        // fuzz当前状态
        unsigned char status;

        // queue长度
        unsigned int queue_len;

        // queue数组
        queue_entry queue[MAX_QUEUE_LEN];

        // 操作类型
        unsigned char op;

        // 我们需要对queue进行排序，所以需要通过数组的形式传给fuzz，
        // 让其根据arranged_idx数组里的信息进行排序，假设数组信息为[0,5,1,2,3,4]
        // 意思是将原本第6个放到第2个的位置，剩余的按顺序往后排
        // 操作后的种子下标idx数组
        unsigned int arranged_idx[MAX_QUEUE_LEN];
    };

    //一些工具函数
    void sleep_with_condition(bool condition){
        int cnt = 0;
        while(true){
            if(condition){
                return;
            }else{
                usleep(case_info::SLEEP_INTERVAL_US);
            }
            cnt++;
            if(cnt > MAX_TRY_TIMES){
                throw std::runtime_error("pause fuzzer failed");
            }
        }
    }

    //server端调用的函数，用于向fuzzer发送操作请求
    void op_resume_fuzzer(CaseInfo* case_info){
        try{
            sleep_with_condition(case_info->status != case_info::fuzzer_status::INTERRUPT);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::RESUME_FUZZER;
    }

    void op_pause_fuzzer(CaseInfo* case_info){
        try{
            sleep_with_condition(case_info->status != case_info::fuzzer_status::INTERRUPT);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::PAUSE_FUZZER;
    }

    case_info::status_t get_fuzzer_status(CaseInfo* case_info){
        return case_info->status;
    }

    case_info::op_type_t get_fuzzer_op(CaseInfo* case_info){
        return case_info->op;
    }

};
#endif