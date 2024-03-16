#ifndef CASE_INFO_HPP
#define CASE_INFO_HPP
namespace case_info {
    // fuzzer运行状态
    const unsigned char RUNNING = 1;        //正在运行: 当fuzzer检测到status为此状态时，会继续进行fuzz操作
    const unsigned char PAUSE = 2;          //暂停：当fuzzer监测到此状态时，会立刻停下fuzz
    const unsigned char READY = 3;          //准备：当fuzzer监测到此状态时，会将status置为RUNNING，并继续运行
    const unsigned char INTERRUPT = 4;      //中断：当fuzzer监测到此状态时，会停下手中的工作，查看op要做什么操作，然后进行相应处理后将status置为TASK_FINISHED
    const unsigned char TASK_FINISHED = 5;  //任务完成：当服务端监测到此状态时，就知道fuzzer已经将任务完成了，
                                            //之后可以将status置为READY，让fuzzer继续运行，
                                            //也可以将status置为INTERRUPT，让fuzzer执行下一个任务

    const int MAX_QUEUE_LEN = 3000;         //队列最大长度
    const int MAX_CASE_FNAME_LEN = 512;     //最长文件名

    const unsigned char REFRESH_QUEUE = 1;      //更新队列里种子的信息
    const unsigned char REARRANGE_QUEUE = 2;    //重排种子顺序
    const unsigned char PAUSE_FUZZER = 3;       //暂停fuzz
    const unsigned char RESUME_FUZZER = 4;      //恢复fuzz

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

    
};
#endif