/*
我们的cpp服务器设计为可以同时和多个fuzzer通信，控制他们的行为，
所以我们需要一个fuzzer manager来进行多个fuzz的控制
这里我们将case_info进一步抽象，包装为一个类，同时可以控制多个case_info的类
*/
#ifndef FUZZER_MANAGER_HPP
#define FUZZER_MANAGER_HPP
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include "Fuzzer.hpp"

class FuzzerManager{
private:
    std::unordered_map<std::string, Fuzzer*> fuzzers;
public:
    FuzzerManager(){

    }

    ~FuzzerManager(){
        this->clear_all_fuzzer();
    }

    void add_fuzzer(Fuzzer* fuzzer){
        std::string fuzzer_id = fuzzer->get_fuzzer_id();
        if(this->fuzzers.find(fuzzer_id) != this->fuzzers.end()){
            throw std::runtime_error("Fuzzer with id " + fuzzer_id + " already existed!");
        }
        this->fuzzers[fuzzer_id] = fuzzer;
    }

    Fuzzer* add_fuzzer(std::string fuzzer_id, std::string shared_file_path){
        auto fuzzer = new Fuzzer(std::move(fuzzer_id), std::move(shared_file_path));
        this->add_fuzzer(fuzzer);
        return fuzzer;
    }

    Fuzzer* get_fuzzer(const std::string& fuzzer_id){
        if(this->fuzzers.find(fuzzer_id) == this->fuzzers.end()){
            throw std::runtime_error("Fuzzer with id " + fuzzer_id + " not existed!");
        }
        return this->fuzzers[fuzzer_id];
    }

    void remove_fuzzer(const std::string& fuzzer_id){
        auto it = this->fuzzers.find(fuzzer_id);
        if(it != this->fuzzers.end()){
            it->second->disconnect();
            this->fuzzers.erase(it);
        }
    }

    void remove_fuzzer(std::unordered_map<std::string, Fuzzer*>::iterator it){
        if(it != this->fuzzers.end()){
            it->second->disconnect();
            delete it->second;
            this->fuzzers.erase(it);
        }
    }

    void clear_all_fuzzer(){
        for(auto it = this->fuzzers.begin(); it != this->fuzzers.end(); it++){
            this->remove_fuzzer(it);
        }
    }
};

#endif 