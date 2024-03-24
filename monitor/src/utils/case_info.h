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

#define READ_QUEUE          6
#define WRITE_QUEUE         7
#define PAUSE_FUZZER        8
#define RESUME_FUZZER       9
#define READ_QUEUE_CUR      10
#define WRITE_QUEUE_CUR     11

#define MAX_QUEUE_LEN       10000
#define MAX_CASE_FNAME_LEN  2048
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
    // afl计算出的种子分数
    double perf_score;
    // 用户手动设定的种子分数
    double user_set_perf_score;
};

struct CaseInfo {
    // fuzz当前状态
    unsigned char status;

    // queue长度
    int queue_len;

    // queue数组
    struct case_info_queue_entry queue[MAX_QUEUE_LEN];

    // 当前正在fuzz的种子
    struct case_info_queue_entry queue_cur;

    // 操作类型
    unsigned char op;

};

void init_case_info_queue_entry(struct case_info_queue_entry* q){
    memset(q->fname, 0, MAX_CASE_FNAME_LEN);
    q->len = 0;
    q->cal_failed = 0;
    q->trim_done = 0;
    q->was_fuzzed = 0;
    q->passed_det = 0;
    q->has_new_cov = 0;
    q->favored = 0;
    q->fs_redundant = 0;
    q->exec_us = 0;
    q->handicap = 0;
    q->depth = 0;
    q->distance = 0;
    q->perf_score = -1;
    q->user_set_perf_score = -1;
}

void init_case_info(struct CaseInfo* case_info){
    case_info->queue_len = 0;
    case_info->status = RUNNING;
    case_info->op = READ_QUEUE_CUR;
    init_case_info_queue_entry(&case_info->queue_cur);
    for(int i = 0;i < MAX_QUEUE_LEN;i++){
        init_case_info_queue_entry(&case_info->queue[i]);
    }
}

#endif //AFL_MONITOR_CASE_INFO_H
