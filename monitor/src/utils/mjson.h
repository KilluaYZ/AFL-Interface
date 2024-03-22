//
// Created by killuayz on 24-3-21.
//

#ifndef AFL_MONITOR_MJSON_H
#define AFL_MONITOR_MJSON_H

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

    double getDouble(std::string key){
        return std::stod(nlohmann::to_string(_json_obj.at(key)));
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

    void put(std::string key, int val){
        _json_obj[key] = val;
    }

    void put(std::string key, double val){
        _json_obj[key] = val;
    }

    void put(std::string key, std::string val){
        _json_obj[key] = val;
    }

    void put(std::string key, const char* val){
        _json_obj[key] = std::string(val);
    }

    void put(std::string key, nlohmann::json val){
        _json_obj[key] = val;
    }

    void put(std::string key, mJson& val){
        put(key, val.get_json_obj());
    }

    void push_back(std::string key, int val){
        _json_obj[key].push_back(val);
    }

    void push_back(std::string key, double val){
        _json_obj[key].push_back(val);
    }

    void push_back(std::string key, std::string val){
        _json_obj[key].push_back(val);
    }

    void push_back(std::string key, nlohmann::json val){
        _json_obj[key].push_back(val);
    }

    void push_back(std::string key, mJson val){
        _json_obj[key].push_back(val.get_json_obj());
    }
};

#endif //AFL_MONITOR_MJSON_H
