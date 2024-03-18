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
#include "../utils/json.hpp"
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
            std::stringstream  ss;
            nlohmann::json body_json;
            ss << std::string(*request_body_str);
            ss >> body_json;
            std::string fuzzer_id = nlohmann::to_string(body_json.at("fuzzer_id"));
            this->m_fuzzerManager->get_fuzzer(fuzzer_id)->pause();
            message->statusCode = 200;
            message->message = "pause success";
        }catch(std::runtime_error& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("pause failed: ")+std::string(e.what());
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("pause failed: ")+std::string(e.what());
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
            std::stringstream  ss;
            nlohmann::json body_json;
            ss << std::string(*request_body_str);
            ss >> body_json;
            std::string fuzzer_id = nlohmann::to_string(body_json.at("fuzzer_id"));
            this->m_fuzzerManager->get_fuzzer(fuzzer_id)->resume();
            message->statusCode = 200;
            message->message = "resume success";
        }catch(std::runtime_error& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("resume failed: ")+std::string(e.what());
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("resume failed: ")+std::string(e.what());
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
            std::stringstream  ss;
            nlohmann::json body_json;
            ss << std::string(*request_body_str);
            ss >> body_json;
            std::string fuzzer_id = nlohmann::to_string(body_json.at("fuzzer_id"));
            std::string shared_file_path = nlohmann::to_string(body_json.at("shared_file_path"));
            auto fuzzer = m_fuzzerManager->add_fuzzer(fuzzer_id, shared_file_path);
            fuzzer->connect();
            message->statusCode = 200;
            message->message = "add success";
        }catch(std::runtime_error& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("add failed: ")+std::string(e.what());
        }catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = std::string("add failed: ")+std::string(e.what());
        }

        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

#endif 