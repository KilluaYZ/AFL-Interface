#include "Fuzzer.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include <exception>

Fuzzer::Fuzzer(std::string fuzzer_id, std::string shared_file_path){
    this->fuzzer_id = fuzzer_id;
    this->shared_file_path = shared_file_path;
    this->case_info = NULL;
}

Fuzzer::~Fuzzer(){
    this->disconnect();
}

void Fuzzer::connect(){
    int fd;
    if((fd = open(".case_info_file", O_RDWR, 0666)) < 0){
        throw std::runtime_error("Fuzzer::connect() open .case_info_file failed");
    }
    ftruncate(fd, sizeof(case_info::CaseInfo));
    case_info = (case_info::CaseInfo*)mmap(NULL, sizeof(case_info::CaseInfo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(case_info == MAP_FAILED){
        throw std::runtime_error("Fuzzer::connect() mmap failed");
    }
    close(fd);
    
}

void Fuzzer::disconnect(){
    if(this->case_info != NULL){
        unlink(this->shared_file_path.c_str());
        munmap(case_info, sizeof(case_info::CaseInfo));
        this->case_info = NULL;
    }
}

void Fuzzer::pause(){
    if(this->case_info == NULL) throw std::runtime_error("Fuzzer::pause() case_info is NULL");
    case_info::op_pause_fuzzer(this->case_info);
}

void Fuzzer::resume(){
    if(this->case_info == NULL) throw std::runtime_error("Fuzzer::resume() case_info is NULL");
    case_info::op_resume_fuzzer(this->case_info);
}

void Fuzzer::stop(){

}

std::string Fuzzer::get_fuzzer_id(){
    return this->fuzzer_id;
}

std::string Fuzzer::get_shared_file_path(){
    return this->shared_file_path;
}

case_info::status_t Fuzzer::get_status(){
    if(this->case_info == NULL) throw std::runtime_error("Fuzzer::get_status() case_info is NULL");
    return case_info::get_fuzzer_status(this->case_info);
}

case_info::op_type_t Fuzzer::get_op(){
    if(this->case_info == NULL) throw std::runtime_error("Fuzzer::get_op() case_info is NULL");
    return case_info::get_fuzzer_op(this->case_info);
}