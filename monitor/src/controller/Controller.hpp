#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "../dto/MsgDto.hpp"
#include "../utils/case_info.hpp"
#include "../FuzzerManager/FuzzerManager.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "../utils/mjson.hpp"

class PauseFuzzHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    PauseFuzzHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        try{
            auto request_body_str = request->readBodyToString();
            mJson mj;
            mj.loads(request_body_str);
            std::string fuzzer_id = mj.getString("fuzzer_id");
            this->m_fuzzerManager->get_fuzzer(fuzzer_id)->pause();
            message->statusCode = 200;
            message->message = "pause success";
            OATPP_LOGI("AFL_MONITOR", "visiting /pause with code %d", 200);
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("pause failed: ")+std::string(e.what());
            OATPP_LOGE("AFL_MONITOR", "visiting /pause with code %d", 500);
        }

        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

class ResumeFuzzHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    ResumeFuzzHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        try{
            auto request_body_str = request->readBodyToString();
            mJson mj;
            mj.loads(request_body_str);
            std::string fuzzer_id = mj.getString("fuzzer_id");
            this->m_fuzzerManager->get_fuzzer(fuzzer_id)->resume();
            message->statusCode = 200;
            message->message = "resume success";
            OATPP_LOGI("AFL_MONITOR", "visiting /resume with code %d", 200);
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("resume failed: ")+std::string(e.what());
            OATPP_LOGE("AFL_MONITOR", "visiting /resume with code %d", 500);
        }

        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

class AddFuzzerHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    AddFuzzerHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        try{
            auto request_body_str = request->readBodyToString();
            mJson mj;
            mj.loads(request_body_str);
            std::string fuzzer_id = mj.getString("fuzzer_id");
            std::string shared_file_path = mj.getString("shared_file_path");
            std::cout<<"shared_file_path = "<<shared_file_path<<std::endl;
            auto fuzzer = m_fuzzerManager->add_fuzzer(fuzzer_id, shared_file_path);
            fuzzer->connect();
            message->statusCode = 200;
            message->message = "add success";
            OATPP_LOGI("AFL_MONITOR", "visiting /add with code %d", 200);
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("add failed: ")+std::string(e.what());
            OATPP_LOGE("AFL_MONITOR", "visiting /add with code %d", 500);
        }

        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

class ListQueueHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    ListQueueHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        try{
            auto request_body_str = request->readBodyToString();
            mJson mj;
            mj.loads(request_body_str);
            std::string fuzzer_id = mj.getString("fuzzer_id");
            auto fuzzer = this->m_fuzzerManager->get_fuzzer(fuzzer_id);
            mJson msgJson =  fuzzer->to_json();
            message->statusCode = 200;
            message->message = msgJson.dumps();
            OATPP_LOGI("AFL_MONITOR", "visiting /list with code %d", 200);
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("list failed: ")+std::string(e.what());
            OATPP_LOGE("AFL_MONITOR", "visiting /list with code %d", 500);
        }
        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};


class ArrangeQueueHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    ArrangeQueueHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        try{
            auto request_body_str = request->readBodyToString();
            mJson mj;
            mj.loads(request_body_str);
            std::string fuzzer_id = mj.getString("fuzzer_id");
            mJson arrange_idx = mj.getJson("arrange_idx");
            auto fuzzer = this->m_fuzzerManager->get_fuzzer(fuzzer_id);
            fuzzer->setArrangeIdx(arrange_idx);
            message->statusCode = 200;
            message->message = "arrange success";
            OATPP_LOGI("AFL_MONITOR", "visiting /arrange with code %d", 200);
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("arrange failed: ")+std::string(e.what());
            OATPP_LOGE("AFL_MONITOR", "visiting /list with code %d", 500);
        }
        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

#endif 