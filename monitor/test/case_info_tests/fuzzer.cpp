#include "../../src/utils/case_info.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <sys/mman.h>
using namespace std;
case_info::CaseInfo* mm;


void sys_err(char* s){
    perror(s);
    exit(1);
}

vector<case_info::queue_entry> queue;

void init_queue(){
    for(int i = 0;i < 5; i++){
        case_info::queue_entry entry;
        entry.distance = 10 * 123.0 + 5.0 * i;
        strcpy(entry.fname, (string("queue_case_test_")+ to_string(i)).c_str());
        entry.len = strlen(entry.fname);
        queue.push_back(entry);
    }
}

void sort_queue(){
    vector<case_info::queue_entry> res;
    for(int i = 0;i < mm->queue_len; i++){
        res.push_back(queue[mm->arranged_idx[i]]);
    }
    queue = res;
}

void write_to_mm(){
    for(int i = 0;i < queue.size(); i++){
        strcpy(mm->queue[i].fname, queue[i].fname);
        mm->queue[i].distance = queue[i].distance;
        mm->queue[i].len = queue[i].len;
    }
    mm->queue_len = queue.size();
}

void add_queue(){
    case_info::queue_entry entry;
    entry.distance = 10 * 123.0 + 5.0 * queue.size();
    strcpy(entry.fname, (string("queue_case_test_")+ to_string(queue.size())).c_str());
    entry.len = strlen(entry.fname);
    queue.push_back(entry);
}

int main(){
    init_queue();
    int fd;
    if((fd = open(".case_info_file", O_RDWR| O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        sys_err("open case_info_file failed");
    }
    ftruncate(fd, sizeof(case_info::CaseInfo));
    mm = (case_info::CaseInfo*)mmap(NULL, sizeof(case_info::CaseInfo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(mm == MAP_FAILED){
        sys_err("mmap failed");
    }
    close(fd);
    int cnt = 0;
    while(1){
        if(mm->status == case_info::fuzzer_status::READY){
            std::cout<<"[READY] -> [RUNNING]"<<std::endl;
            mm->status = case_info::fuzzer_status::RUNNING;
        }else if(mm->status == case_info::fuzzer_status::RUNNING){

        }else if(mm->status == case_info::fuzzer_status::INTERRUPT){
            std::cout<<"[INTERUPT]"<<std::endl;
            if(mm->op == case_info::op_type::PAUSE_FUZZER){
                std::cout<<"PAUSE_FUZZER"<<std::endl;
                std::cout<<"[INTERUPT] -> [PAUSE]"<<std::endl;
                mm->status = case_info::fuzzer_status::PAUSE;
            }else if(mm->op == case_info::op_type::RESUME_FUZZER){
                std::cout<<"RESUME_FUZZER"<<std::endl;
                std::cout<<"[INTERUPT] -> [READY]"<<std::endl;
                mm->status = case_info::fuzzer_status::READY;
            }else if(mm->op == case_info::op_type::REFRESH_QUEUE){
                std::cout<<"REFRESH_QUEUE"<<std::endl;
                write_to_mm();
                std::cout<<"[INTERUPT] -> [TASK_FINISHED]"<<std::endl;
                mm->status = case_info::fuzzer_status::TASK_FINISHED;
            }else if(mm->op == case_info::op_type::REARRANGE_QUEUE){
                std::cout<<"REARRANGE_QUEUE"<<std::endl;
                sort_queue();
                mm->status = case_info::fuzzer_status::TASK_FINISHED;
            }
            continue;
        }else if(mm->status == case_info::fuzzer_status::TASK_FINISHED){
            std::cout<<"[TASK_FINISHED] -> [RUNNING]"<<std::endl;
            mm->status = case_info::fuzzer_status::PAUSE;
        }else if(mm->status == case_info::fuzzer_status::PAUSE){
            usleep(1000);
            continue;
        }
        add_queue();
        std::cout<<"current timestamp = "<<cnt++<<std::endl;
        sleep(5);
    }

    return 0;
}