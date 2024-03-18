#include "FuzzerManager.hpp"

FuzzerManager::FuzzerManager(){

}

FuzzerManager::~FuzzerManager(){
    this->clear_all_fuzzer();
}

void FuzzerManager::add_fuzzer(Fuzzer* fuzzer){
    std::string fuzzer_id = fuzzer->get_fuzzer_id();
    if(this->fuzzers.find(fuzzer_id) != this->fuzzers.end()){
        throw std::runtime_error("Fuzzer with id " + fuzzer_id + " already existed!");
    }
    this->fuzzers[fuzzer_id] = fuzzer;
}

Fuzzer* FuzzerManager::add_fuzzer(std::string fuzzer_id, std::string shared_file_path){
    auto fuzzer = new Fuzzer(fuzzer_id, shared_file_path);
    this->add_fuzzer(fuzzer);
    return fuzzer;
}

Fuzzer* FuzzerManager::get_fuzzer(std::string fuzzer_id){
    if(this->fuzzers.find(fuzzer_id) == this->fuzzers.end()){
        throw std::runtime_error("Fuzzer with id " + fuzzer_id + " not existed!");
    }
    return this->fuzzers[fuzzer_id];
}

void FuzzerManager::remove_fuzzer(std::string fuzzer_id){
    auto it = this->fuzzers.find(fuzzer_id);
    if(it != this->fuzzers.end()){
        it->second->disconnect();
        this->fuzzers.erase(it);
    }
}

void FuzzerManager::remove_fuzzer(std::unordered_map<std::string, Fuzzer*>::iterator it){
    if(it != this->fuzzers.end()){
        it->second->disconnect();
        delete it->second;
        this->fuzzers.erase(it);
    }
}

void FuzzerManager::clear_all_fuzzer(){
    for(auto it = this->fuzzers.begin(); it != this->fuzzers.end(); it++){
        this->remove_fuzzer(it);
    }
}

