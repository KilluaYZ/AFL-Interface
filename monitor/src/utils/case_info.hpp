#ifndef CASE_INFO_HPP
#define CASE_INFO_HPP
#include <stdexcept>
#include <utility>
#include <unistd.h>
#include "mjson.hpp"
#include "cstring"
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
        char fname[MAX_CASE_FNAME_LEN] = {0};
        // 种子长度
        int len = 0;

        // 校准是否失败（Calibration failed?）
        unsigned char cal_filed = 0;
        // 是否经过剪枝
        unsigned char trim_done = 0;
        // 是否经过fuzz
        unsigned char was_fuzzed = 0;
        // 是否通过重要步骤（Deterministic stage passed?）
        unsigned char passed_det = 0;
        // 是否产生新的覆盖
        unsigned char has_new_cov = 0;
        // 是否是感兴趣的种子
        unsigned char favored = 0;
        // 是否被文件系统标注为冗余的
        unsigned char fs_redundant = 0;

        // 种子执行时间(us)
        unsigned long long int exec_us = 0;
        // 落后的队列周期数(Number of queue cycles behind)
        unsigned long long int handicap = 0;
        // 路径深度
        unsigned long long int depth = 0;

        // aflgo计算得出的语法距离
        double distance = 0;
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

    mJson arranged_idx_to_json(CaseInfo* case_info){
        mJson jres;
        for(int i = 0;i < case_info->queue_len; i++){
            jres.push_backInt(case_info->arranged_idx[i]);
        }
        return jres;
    }

    void arranged_idx_from_json(CaseInfo* case_info, mJson& arranged_idx_json){
        auto v = arranged_idx_json.getArray();
        for(int i = 0;i < v.size();i++){
            case_info->arranged_idx[i] = v[i].getInt();
        }
    }

    mJson queue_entry_to_json(queue_entry* qe){
        mJson j;
        if(qe != nullptr){
            j.putString("fname", std::string(qe->fname));
            j.putInt("len", qe->len);
            j.putInt("cal_filed", (int)qe->cal_filed);
            j.putInt("trim_done", (int)qe->trim_done);
            j.putInt("was_fuzzed", (int)qe->was_fuzzed);
            j.putInt("passed_det", (int)qe->passed_det);
            j.putInt("has_new_cov", (int)qe->has_new_cov);
            j.putInt("favored", (int)qe->favored);
            j.putInt("fs_redundant", (int)qe->fs_redundant);
            j.putLongLong("exec_us", qe->exec_us);
            j.putLongLong("handicap", qe->handicap);
            j.putLongLong("depth", qe->depth);
            j.putDouble("distance", qe->distance);
        }
        return j;
    }

    void queue_entry_from_json(queue_entry* qe, mJson queue_entry_json){
        auto tmp_p = queue_entry_json.getString("fname").c_str();
        auto tmp_size = queue_entry_json.getString("fname").size();
        strncpy(qe->fname, tmp_p, tmp_size*sizeof(char));

        qe->len = queue_entry_json.getInt("len");
        qe->cal_filed = queue_entry_json.getInt("cal_filed");
        qe->trim_done = queue_entry_json.getInt("trim_done");
        qe->was_fuzzed = queue_entry_json.getInt("was_fuzzed");
        qe->passed_det = queue_entry_json.getInt("passed_det");
        qe->has_new_cov = queue_entry_json.getInt("has_new_cov");
        qe->favored = queue_entry_json.getInt("favored");
        qe->fs_redundant = queue_entry_json.getInt("fs_redundant");
        qe->exec_us = queue_entry_json.getLongLongInt("exec_us");
        qe->handicap = queue_entry_json.getLongLongInt("handicap");
        qe->depth = queue_entry_json.getLongLongInt("depth");
        qe->distance = queue_entry_json.getDouble("distance");
    }

    void queue_entry_from_json(queue_entry* qe, std::string queue_entry_str){
        mJson j;
        j.loads(std::move(queue_entry_str));
        queue_entry_from_json(qe,j);
    }

    mJson queue_to_json(CaseInfo* case_info){
        mJson j;
        if(case_info != nullptr){
            for(int i = 0;i < case_info->queue_len; i++){
                auto tj= queue_entry_to_json(case_info->queue+i);
                j.push_backJson(  tj);
            }
        }
        return j;
    }

    void queue_from_json(CaseInfo* case_info, mJson& queue_info_json){
        auto j = queue_info_json.getArray("queue");
        int cnt = 0;
        for(auto & queue_entry_json : j){
            queue_entry_from_json(case_info->queue+cnt, queue_entry_json);
            cnt++;
        }
        case_info->queue_len  =  j.size();
    }

    void queue_from_json(CaseInfo* case_info, const std::string& queue_info_str){
        mJson j;
        j.loads(queue_info_str);
        queue_from_json(case_info, j);
    }

    case_info::status_t getFuzzerStatus(CaseInfo* case_info){
        return case_info->status;
    }

    std::string getFuzzerStatusString(CaseInfo* case_info){
        case_info::status_t fuzzer_status = getFuzzerStatus(case_info);
        std::string res;
        if(fuzzer_status == case_info::fuzzer_status::READY){
            res = "ready";
        }else if(fuzzer_status == case_info::fuzzer_status::RUNNING){
            res = "running";
        }else if(fuzzer_status == case_info::fuzzer_status::INTERRUPT){
            res = "interrupt";
        }else if(fuzzer_status == case_info::fuzzer_status::PAUSE){
            res = "pause";
        }else if(fuzzer_status == case_info::fuzzer_status::TASK_FINISHED){
            res = "task_finished";
        }
        return res;
    }

    case_info::op_type_t getOpType(CaseInfo* case_info){
        return case_info->op;
    }

    std::string getOpTypeString(CaseInfo* case_info){
        case_info::op_type_t op = getOpType(case_info);
        std::string res;
        if(op ==  case_info::op_type::PAUSE_FUZZER){
            res = "pause_fuzzer";
        }else if(op ==  case_info::op_type::RESUME_FUZZER){
            res = "resume_fuzzer";
        }else if(op ==  case_info::op_type::REARRANGE_QUEUE){
            res = "rearrange_queue";
        }else if(op ==  case_info::op_type::REFRESH_QUEUE){
            res = "refresh_queue";
        }
        return res;
    }

    void setOpTypeByString(CaseInfo* case_info, std::string op_type){
        if(op_type == "pause_fuzzer"){
            case_info->op = case_info::op_type::PAUSE_FUZZER;
        }else if(op_type == "resume_fuzzer"){
            case_info->op = case_info::op_type::RESUME_FUZZER;
        }else if(op_type == "rearrange_queue"){
            case_info->op = case_info::op_type::REARRANGE_QUEUE;
        }else if(op_type == "refresh_queue"){
            case_info->op = case_info::op_type::REFRESH_QUEUE;
        }
    }

    mJson to_json(CaseInfo* case_info){
        mJson j;
        std::string fuzzer_status = getFuzzerStatusString(case_info);
        std::string op_type = getOpTypeString(case_info);
        mJson queue = queue_to_json(case_info);
        mJson arranged_idx = arranged_idx_to_json(case_info);
        int queue_len = case_info->queue_len;
        j.putString("status", fuzzer_status);
        j.putString("op", op_type);
        j.putJson("queue", queue);
        j.putJson("arranged_idx", arranged_idx);
        j.putInt("queue_len", queue_len);
        return j;
    }

    void from_json(CaseInfo* case_info, mJson& case_info_json){
        std::string op_type = case_info_json.getString("op");
        mJson queue = case_info_json.getJson("queue");
        mJson arranged_idx = case_info_json.getJson("arranged_idx");
        int queue_len = case_info_json.getInt("queue_len");
        setOpTypeByString(case_info, op_type);
        queue_from_json(case_info, queue);
        arranged_idx_from_json(case_info, arranged_idx);
        case_info->queue_len = queue_len;
    }

    void from_json(CaseInfo* case_info, const std::string& case_info_str){
        mJson j;
        j.loads(case_info_str);
        from_json(case_info, j);
    }

    //一些工具函数
    void sleep_if_status_is_interrupt(CaseInfo* case_info, int try_times = MAX_TRY_TIMES, int sleep_interval_us  = SLEEP_INTERVAL_US){
        int cnt = 0;
        while(true){
            if(case_info->status != case_info::fuzzer_status::INTERRUPT){
                return;
            }else{
                usleep(sleep_interval_us);
            }
            cnt++;
            if(cnt > try_times){
                throw std::runtime_error("fuzzer failed");
            }
        }
    }

    //server端调用的函数，用于向fuzzer发送操作请求
    void op_resume_fuzzer(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::RESUME_FUZZER;
    }

    void op_pause_fuzzer(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::PAUSE_FUZZER;
    }

    void op_refresh_queue(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch (std::runtime_error& e){
            return;
        }
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::REFRESH_QUEUE;
    }

    void op_rearrange_queue(CaseInfo* case_info, mJson arrangeIdx){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch (std::runtime_error& e){
            return;
        }
        arranged_idx_from_json(case_info, arrangeIdx);
        case_info->status = case_info::fuzzer_status::INTERRUPT;
        case_info->op = case_info::op_type::REARRANGE_QUEUE;
    }

    case_info::status_t get_fuzzer_status(CaseInfo* case_info){
        return case_info->status;
    }

    case_info::op_type_t get_fuzzer_op(CaseInfo* case_info){
        return case_info->op;
    }

};
#endif