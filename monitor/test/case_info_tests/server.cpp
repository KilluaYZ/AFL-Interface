#include "../../src/utils/case_info.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>

int main(){
    int fd;
    case_info::CaseInfo* mm;
    if((fd = open("tmpfile", O_RDWR, 0666)) < 0){
        perror("failed to open tmpfile");
        exit(1);
    }

    ftruncate(fd, sizeof(case_info::CaseInfo));
    mm = (case_info::CaseInfo*)mmap(NULL, sizeof(case_info::CaseInfo), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if(mm == MAP_FAILED) {
        perror("failed to open tmpfile");
        exit(1);
    }
    close(fd);
    while(1){
        int in;
        std::cout<<"[Input]: ";
        std::cin>>in;
        if(in == 0){
            case_info::op_pause_fuzzer(mm);
        }else if(in == 1){
            case_info::op_resume_fuzzer(mm);
        }else if(in == 2){
            break;
        }
    }
    unlink("tmpfile");
    munmap(mm, sizeof(case_info::CaseInfo));
    return 0;
}