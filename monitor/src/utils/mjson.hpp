//
// Created by killuayz on 24-3-21.
//

#ifndef AFL_MONITOR_MJSON_HPP
#define AFL_MONITOR_MJSON_HPP

#include "json.hpp"
#include <string>
#include <vector>

class mJson{
private:
    nlohmann::json _json_obj;

public:
    mJson(){}

    mJson(nlohmann::json json_obj){
        _json_obj = json_obj;
    }
    ~mJson(){}

    nlohmann::json get_json_obj(){
        return _json_obj;
    }

    std::string dumps(){
        return _json_obj.dump();
    }

    void loads(std::string json_str){
        _json_obj = nlohmann::json::parse(json_str.c_str());
    }

    std::string getString(std::string key){
        std::string res = nlohmann::to_string(_json_obj.at(key));
        if(res.size() > 0 && res[0] == '"' && res[res.size()-1] == '"')
            return res.substr(1, res.size() - 2);
        return res;
    }

    int getInt(std::string key){
        return std::stoi(nlohmann::to_string(_json_obj.at(key)));
    }

    int getLongLongInt(std::string key){
        return std::stoll(nlohmann::to_string(_json_obj.at(key)));
    }

    double getDouble(std::string key){
        return std::stod(nlohmann::to_string(_json_obj.at(key)));
    }

    std::string getString(){
        std::string res = nlohmann::to_string(_json_obj);
        if(res.size() > 0 && res[0] == '"' && res[res.size()-1] == '"')
            return res.substr(1, res.size() - 2);
        return res;
    }

    int getInt(){
        return std::stoi(nlohmann::to_string(_json_obj));
    }

    int getLongLongInt(){
        return std::stoll(nlohmann::to_string(_json_obj));
    }

    double getDouble(){
        return std::stod(nlohmann::to_string(_json_obj));
    }

    mJson getJson(std::string key){
        nlohmann::json res = _json_obj.at(key);
        return mJson(res);
    }

    std::vector<mJson> getArray(std::string key){
        std::vector<mJson> res;
        for(auto &j : _json_obj[key]){
            res.push_back(mJson(j));
        }
        return res;
    }

    std::vector<mJson> getArray(){
        std::vector<mJson> res;
        for(auto &j : _json_obj){
            res.push_back(mJson(j));
        }
        return res;
    }

    void putInt(std::string key, int val){
        _json_obj[key] = val;
    }

    void putLongLong(std::string key, long long int val){
        _json_obj[key] = val;
    }

    void putDouble(std::string key, double val){
        _json_obj[key] = val;
    }

    void putString(std::string key, std::string val){
        _json_obj[key] = val;
    }

    void putString(std::string key, const char* val){
        _json_obj[key] = std::string(val);
    }

    void putJson(std::string key, nlohmann::json val){
        _json_obj[key] = val;
    }

    void putJson(std::string key, mJson& val){
        putJson(key, val.get_json_obj());
    }

    void push_backInt(std::string key, int val){
        _json_obj[key].push_back(val);
    }

    void push_backLongLong(std::string key, long long int val){
        _json_obj[key].push_back(val);
    }

    void push_backDouble(std::string key, double val){
        _json_obj[key].push_back(val);
    }

    void push_backString(std::string key, std::string val){
        _json_obj[key].push_back(val);
    }

    void push_backJson(std::string key, nlohmann::json val){
        _json_obj[key].push_back(val);
    }

    void push_backJson(std::string key, mJson val){
        _json_obj[key].push_back(val.get_json_obj());
    }

    void push_backInt(int val){
        _json_obj.push_back(val);
    }

    void push_backLongLong(long long int val){
        _json_obj.push_back(val);
    }

    void push_backDouble(double val){
        _json_obj.push_back(val);
    }

    void push_backString(std::string val){
        _json_obj.push_back(val);
    }

    void push_backJson(nlohmann::json val){
        _json_obj.push_back(val);
    }

    void push_backJson(mJson val){
        _json_obj.push_back(val.get_json_obj());
    }

    int size(){
        return _json_obj.size();
    }

};

#endif //AFL_MONITOR_MJSON_HPP
