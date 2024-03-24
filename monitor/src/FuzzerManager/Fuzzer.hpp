/*
该文件对单个case_info进行抽象包装，将与fuzzer通信的方法进行包装，
是一个可以操作fuzzer行为的类，对应一个正在运行的fuzzer
*/
#ifndef FUZZER_HPP
#define FUZZER_HPP
#include "../utils/case_info.h"
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
    CaseInfo* case_info;
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
        ftruncate(fd, sizeof(CaseInfo));
        case_info = (CaseInfo*)mmap(nullptr, sizeof(CaseInfo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(case_info == MAP_FAILED){
            throw std::runtime_error("Fuzzer::connect() mmap failed");
        }
        close(fd);

    }

    void disconnect(){
        if(this->case_info != nullptr){
            unlink(this->shared_file_path.c_str());
            munmap(case_info, sizeof(CaseInfo));
            this->case_info = nullptr;
        }
    }

    void pause(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::pause() case_info is NULL");
        op_pause_fuzzer(this->case_info);
    }

    void resume(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::resume() case_info is NULL");
        op_resume_fuzzer(this->case_info);
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
        return getFuzzerStatusString(this->case_info);
    }

    std::string get_op(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::get_op() case_info is NULL");
        return getOpTypeString(this->case_info);
    }

    mJson to_json(){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::to_json() case_info is NULL");
        try{
            op_refresh_queue(case_info);
            sleep_if_status_is_interrupt(case_info, 100000);
        }catch (std::exception& e){
            std::stringstream ss;
            ss<<"Fuzzer::to_json() reach time limit..."<<e.what()<<std::endl;
            std::cout<<ss.str()<<std::endl;
            throw std::runtime_error(ss.str());
        }
        return to_json(case_info);
    }

    void setArrangeIdx(mJson& json_obj){
        if(this->case_info == nullptr) throw std::runtime_error("Fuzzer::setArrangeIdx() case_info is NULL");
        try{
            op_rearrange_queue(this->case_info, json_obj);
            sleep_if_status_is_interrupt(case_info, 30000000);
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


    // static method
    static mJson arranged_idx_to_json(CaseInfo* case_info){
        mJson jres;
        for(int i = 0;i < case_info->queue_len; i++){
            jres.push_backInt(case_info->arranged_idx[i]);
        }
        return jres;
    }

    static void arranged_idx_from_json(CaseInfo* case_info, mJson& arranged_idx_json){
        auto v = arranged_idx_json.getArray();
        for(int i = 0;i < v.size();i++){
            case_info->arranged_idx[i] = v[i].getInt();
        }
    }

    static mJson queue_entry_to_json(case_info_queue_entry* qe){
        mJson j;
        if(qe != nullptr){
            j.putString("fname", std::string(qe->fname));
            j.putInt("len", qe->len);
            j.putInt("cal_filed", (int)qe->cal_failed);
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

    static void queue_entry_from_json(case_info_queue_entry* qe, mJson queue_entry_json){
        auto tmp_p = queue_entry_json.getString("fname").c_str();
        auto tmp_size = queue_entry_json.getString("fname").size();
        strncpy(qe->fname, tmp_p, tmp_size*sizeof(char));

        qe->len = queue_entry_json.getInt("len");
        qe->cal_failed = queue_entry_json.getInt("cal_filed");
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

    static void queue_entry_from_json(case_info_queue_entry* qe, std::string queue_entry_str){
        mJson j;
        j.loads(std::move(queue_entry_str));
        queue_entry_from_json(qe,j);
    }

    static mJson queue_to_json(CaseInfo* case_info){
        mJson j;
        if(case_info != nullptr){
            for(int i = 0;i < case_info->queue_len; i++){
                auto tj= queue_entry_to_json(case_info->queue+i);
                j.push_backJson(  tj);
            }
        }
        return j;
    }

    static void queue_from_json(CaseInfo* case_info, mJson& queue_info_json){
        auto j = queue_info_json.getArray("queue");
        int cnt = 0;
        for(auto & queue_entry_json : j){
            queue_entry_from_json(case_info->queue+cnt, queue_entry_json);
            cnt++;
        }
        case_info->queue_len  =  j.size();
    }

    static void queue_from_json(CaseInfo* case_info, const std::string& queue_info_str){
        mJson j;
        j.loads(queue_info_str);
        queue_from_json(case_info, j);
    }

    static status_t getFuzzerStatus(CaseInfo* case_info){
        return case_info->status;
    }

    static std::string getFuzzerStatusString(CaseInfo* case_info){
        status_t fuzzer_status = getFuzzerStatus(case_info);
        std::string res;
        if(fuzzer_status == READY){
            res = "ready";
        }else if(fuzzer_status == RUNNING){
            res = "running";
        }else if(fuzzer_status == INTERRUPT){
            res = "interrupt";
        }else if(fuzzer_status == PAUSE){
            res = "pause";
        }else if(fuzzer_status == TASK_FINISHED){
            res = "task_finished";
        }
        return res;
    }

    static op_type_t getOpType(CaseInfo* case_info){
        return case_info->op;
    }

    static std::string getOpTypeString(CaseInfo* case_info){
        op_type_t op = getOpType(case_info);
        std::string res;
        if(op == PAUSE_FUZZER){
            res = "pause_fuzzer";
        }else if(op == RESUME_FUZZER){
            res = "resume_fuzzer";
        }else if(op == REARRANGE_QUEUE){
            res = "rearrange_queue";
        }else if(op == REFRESH_QUEUE){
            res = "refresh_queue";
        }
        return res;
    }

    static void setOpTypeByString(CaseInfo* case_info, std::string op_type){
        if(op_type == "pause_fuzzer"){
            case_info->op = PAUSE_FUZZER;
        }else if(op_type == "resume_fuzzer"){
            case_info->op = RESUME_FUZZER;
        }else if(op_type == "rearrange_queue"){
            case_info->op = REARRANGE_QUEUE;
        }else if(op_type == "refresh_queue"){
            case_info->op = REFRESH_QUEUE;
        }
    }

    static mJson to_json(CaseInfo* case_info){
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

    static void from_json(CaseInfo* case_info, mJson& case_info_json){
        std::string op_type = case_info_json.getString("op");
        mJson queue = case_info_json.getJson("queue");
        mJson arranged_idx = case_info_json.getJson("arranged_idx");
        int queue_len = case_info_json.getInt("queue_len");
        setOpTypeByString(case_info, op_type);
        queue_from_json(case_info, queue);
        arranged_idx_from_json(case_info, arranged_idx);
        case_info->queue_len = queue_len;
    }

    static void from_json(CaseInfo* case_info, const std::string& case_info_str){
        mJson j;
        j.loads(case_info_str);
        from_json(case_info, j);
    }

    //一些工具函数
    static void sleep_if_status_is_interrupt(CaseInfo* case_info, int try_times = MAX_TRY_TIMES, int sleep_interval_us  = SLEEP_INTERVAL_US){
        int cnt = 0;
        while(true){
            if(case_info->status != INTERRUPT){
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
    static void op_resume_fuzzer(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = INTERRUPT;
        case_info->op = RESUME_FUZZER;
    }

    static void op_pause_fuzzer(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch(std::runtime_error& e){
            return;
        }
        case_info->status = INTERRUPT;
        case_info->op = PAUSE_FUZZER;
    }

    static void op_refresh_queue(CaseInfo* case_info){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch (std::runtime_error& e){
            return;
        }
        case_info->status = INTERRUPT;
        case_info->op = REFRESH_QUEUE;
    }

    static void op_rearrange_queue(CaseInfo* case_info, mJson arrangeIdx){
        try{
            sleep_if_status_is_interrupt(case_info);
        }catch (std::runtime_error& e){
            return;
        }
        arranged_idx_from_json(case_info, arrangeIdx);
        case_info->status = INTERRUPT;
        case_info->op = REARRANGE_QUEUE;
    }

};

#endif 