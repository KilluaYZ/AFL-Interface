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
#include <utility>
#include "../utils/mjson.hpp"
#include <sstream>

class Fuzzer {
private:
    case_info::CaseInfo* case_info;
    std::string fuzzer_id;
    std::string shared_file_path;

public:
    Fuzzer(std::string fuzzer_id, std::string shared_file_path){
        this->fuzzer_id = std::move(fuzzer_id);
        this->shared_file_path = std::move(shared_file_path);
        this->case_info = nullptr;
    }

    ~Fuzzer(){
        this->disconnect();
    }

    void connect(){
        int fd;
        if((fd = open((this->shared_file_path+"/.case_info_file").c_str(), O_RDWR, O_RDWR| O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
            throw std::runtime_error("Fuzzer::connect() open " + this->shared_file_path+"/.case_info_file failed.");
        }
        ftruncate(fd, sizeof(case_info::CaseInfo));
        case_info = (case_info::CaseInfo*)mmap(nullptr, sizeof(case_info::CaseInfo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(case_info == MAP_FAILED){
            throw std::runtime_error("Fuzzer::connect() mmap failed");
        }
        close(fd);

    }

    void disconnect(){
        if(this->case_info != nullptr){
            unlink(this->shared_file_path.c_str());
            munmap(case_info, sizeof(case_info::CaseInfo));
            this->case_info = nullptr;
        }
    }

    void pause(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::pause() case_info is NULL");
        case_info::op_pause_fuzzer(this->case_info);
    }

    void resume(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::resume() case_info is NULL");
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

    std::string get_status(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::get_status() case_info is NULL");
        return case_info::getFuzzerStatusString(this->case_info);
    }

    std::string get_op(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::get_op() case_info is NULL");
        return case_info::getOpTypeString(this->case_info);
    }

    mJson to_json(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::to_json() case_info is NULL");
        try{
            case_info::op_refresh_queue(case_info);
            case_info::sleep_if_status_is_interrupt(case_info, 100000);
        }catch (std::exception& e){
            std::stringstream ss;
            ss<<"Fuzzer::to_json() reach time limit..."<<e.what()<<std::endl;
            std::cout<<ss.str()<<std::endl;
            throw std::runtime_error(ss.str());
        }
        return case_info::to_json(case_info);
    }

    void setArrangeIdx(mJson& json_obj){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::setArrangeIdx() case_info is NULL");
        try{
            case_info::op_rearrange_queue(this->case_info, json_obj);
            case_info::sleep_if_status_is_interrupt(case_info, 30000000);
        }catch (std::exception& e){
            std::stringstream  ss;
            ss<<"Fuzzer::to_json() reach time limit..."<<std::endl<<e.what()<<std::endl;
            std::cout<<ss.str()<<std::endl;
            throw std::runtime_error(ss.str());
        }
    }

    void setArrangeIdx(std::string& json_str){
        mJson j;
        j.loads(json_str);
        setArrangeIdx(j);
    }

};

#endif 