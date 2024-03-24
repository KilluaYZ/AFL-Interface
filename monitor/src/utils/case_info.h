//
// Created by killuayz on 24-3-23.
//

#ifndef AFL_MONITOR_CASE_INFO_H
#define AFL_MONITOR_CASE_INFO_H
typedef unsigned char status_t;
typedef unsigned char op_type_t;

#define RUNNING             1       //正在运行: 当fuzzer检测到status为此状态时，会继续进行fuzz操作
#define PAUSE               2       //暂停：当fuzzer监测到此状态时，会立刻停下fuzz
#define READY               3       //准备：当fuzzer监测到此状态时，会将status置为RUNNING，并继续运行
#define INTERRUPT           4       //中断：当fuzzer监测到此状态时，会停下手中的工作，查看op要做什么操作，然后进行相应处理后将status置为TASK_FINISHED
#define TASK_FINISHED       5       //任务完成：当服务端监测到此状态时，就知道fuzzer已经将任务完成了，
                                    //之后可以将status置为READY，让fuzzer继续运行
                                    //也可以将status置为INTERRUPT，让fuzzer执行下一个任务

#define REFRESH_QUEUE       6
#define REARRANGE_QUEUE     7
#define PAUSE_FUZZER        8
#define RESUME_FUZZER       9

#define MAX_QUEUE_LEN       3000
#define MAX_CASE_FNAME_LEN  512
#define MAX_TRY_TIMES       100
#define SLEEP_INTERVAL_US   100

struct case_info_queue_entry{
    // 种子文件名
    char fname[MAX_CASE_FNAME_LEN];
    // 种子长度
    int len;

    // 校准是否失败（Calibration failed?）
    unsigned char cal_failed;
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
    struct case_info_queue_entry queue[MAX_QUEUE_LEN];

    // 操作类型
    unsigned char op;

    // 我们需要对queue进行排序，所以需要通过数组的形式传给fuzz，
    // 让其根据arranged_idx数组里的信息进行排序，假设数组信息为[0,5,1,2,3,4]
    // 意思是将原本第6个放到第2个的位置，剩余的按顺序往后排
    // 操作后的种子下标idx数组
    unsigned int arranged_idx[MAX_QUEUE_LEN];
};


#endif //AFL_MONITOR_CASE_INFO_H
