/*
该文件对单个case_info进行抽象包装，将与fuzzer通信的方法进行包装，
是一个可以操作fuzzer行为的类，对应一个正在运行的fuzzer
*/
#ifndef FUZZER_HPP
#define FUZZER_HPP
#include "../utils/case_info.hpp"
#include <string>
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include <exception>

class Fuzzer {
private:
    case_info::CaseInfo* case_info;
    std::string fuzzer_id;
    std::string shared_file_path;

public:
    Fuzzer(std::string fuzzer_id, std::string shared_file_path){
        this->fuzzer_id = fuzzer_id.substr(1, fuzzer_id.size() - 2);
        this->shared_file_path = shared_file_path.substr(1, shared_file_path.size() - 2);
        this->case_info = NULL;
    }

    ~Fuzzer(){
        this->disconnect();
    }

    void connect(){
        int fd;
        if((fd = open((this->shared_file_path+"/.case_info_file").c_str(), O_RDWR, 0666)) < 0){
            throw std::runtime_error("Fuzzer::connect() open " + this->shared_file_path+"/.case_info_file failed.");
        }
        ftruncate(fd, sizeof(case_info::CaseInfo));
        case_info = (case_info::CaseInfo*)mmap(NULL, sizeof(case_info::CaseInfo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(case_info == MAP_FAILED){
            throw std::runtime_error("Fuzzer::connect() mmap failed");
        }
        close(fd);

    }

    void disconnect(){
        if(this->case_info != NULL){
            unlink(this->shared_file_path.c_str());
            munmap(case_info, sizeof(case_info::CaseInfo));
            this->case_info = NULL;
        }
    }

    void pause(){
        if(this->case_info == NULL) throw std::runtime_error("Fuzzer::pause() case_info is NULL");
        case_info::op_pause_fuzzer(this->case_info);
    }

    void resume(){
        if(this->case_info == NULL) throw std::runtime_error("Fuzzer::resume() case_info is NULL");
        case_info::op_resume_fuzzer(this->case_info);
    }

    void stop(){

    }

    std::string get_fuzzer_id(){
        return this->fuzzer_id;
    }

    std::string get_shared_file_path(){
        return this->shared_file_path;
    }

    case_info::status_t get_status(){
        if(this->case_info == NULL) throw std::runtime_error("Fuzzer::get_status() case_info is NULL");
        return case_info::get_fuzzer_status(this->case_info);
    }

    case_info::op_type_t get_op(){
        if(this->case_info == NULL) throw std::runtime_error("Fuzzer::get_op() case_info is NULL");
        return case_info::get_fuzzer_op(this->case_info);
    }
    
};

#endif 