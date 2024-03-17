/*
我们的cpp服务器设计为可以同时和多个fuzzer通信，控制他们的行为，
所以我们需要一个fuzzer manager来进行多个fuzz的控制
这里我们将case_info进一步抽象，包装为一个类，同时可以控制多个case_info的类
*/
#ifndef FUZZER_MANAGER_HPP
#define FUZZER_MANAGER_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include "Fuzzer.hpp"

class FuzzerManager{
private:
    std::unordered_map<std::string, Fuzzer> fuzzers;
public:
    FuzzerManager();
    ~FuzzerManager();
    void add_fuzzer(Fuzzer fuzzer);
    void add_fuzzer(std::string fuzzer_id, std::string shared_file_path);
    Fuzzer get_fuzzer(std::string fuzzer_id);
    void FuzzerManager::remove_fuzzer(std::unordered_map<std::string, Fuzzer>::iterator it);
    void remove_fuzzer(std::string fuzzer_id);
    void clear_all_fuzzer();
};

#endif 