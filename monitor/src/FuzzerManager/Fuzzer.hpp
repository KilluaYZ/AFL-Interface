/*
该文件对单个case_info进行抽象包装，将与fuzzer通信的方法进行包装，
是一个可以操作fuzzer行为的类，对应一个正在运行的fuzzer
*/
#ifndef FUZZER_HPP
#define FUZZER_HPP
#include "../utils/case_info.hpp"
#include <string>

class Fuzzer {
private:
    case_info::CaseInfo* case_info;
    std::string fuzzer_id;
    std::string shared_file_path;

public:
    Fuzzer(std::string fuzzer_id, std::string shared_file_path);
    ~Fuzzer();

    std::string get_fuzzer_id();
    std::string get_shared_file_path();

    // fuzzer信息
    case_info::status_t get_status();
    case_info::op_type_t get_op();

    // fuzzer 操作
    void connect();
    void disconnect();
    void pause();
    void resume();
    void stop();
    
};

#endif 