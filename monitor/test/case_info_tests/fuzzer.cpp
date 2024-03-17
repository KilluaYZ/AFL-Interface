#include "../../src/utils/case_info.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>

void sys_err(char* s){
    perror(s);
    exit(1);
}

int main(){
    int fd;
    case_info::CaseInfo* mm;
    if((fd = open("tmpfile", O_RDWR| O_CREAT | O_TRUNC)) < 0){
        sys_err("open tmpfile failed");
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
            if(mm->op == case_info::op_type::PAUSE_FUZZER){
                std::cout<<"[INTERUPT] -> [PAUSE]"<<std::endl;
                mm->status = case_info::fuzzer_status::PAUSE;
            }else if(mm->op == case_info::op_type::RESUME_FUZZER){
                std::cout<<"[INTERUPT] -> [READY]"<<std::endl;
                mm->status = case_info::fuzzer_status::READY;
            }
            continue;
        }else if(mm->status == case_info::fuzzer_status::TASK_FINISHED){
            std::cout<<"[TASK_FINISHED] -> [RUNNING]"<<std::endl;
            mm->status = case_info::fuzzer_status::RUNNING;
        }else if(mm->status == case_info::fuzzer_status::PAUSE){
            usleep(1000);
            continue;
        }

        std::cout<<"current timestamp = "<<cnt++<<std::endl;
        sleep(1);
    }

    return 0;
}