#ifndef TEST_CONTROLLER_HPP
#define TEST_CONTROLLER_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "../dto/MsgDto.hpp"
#include <iostream>
#include <string>
#include "../utils/json.hpp"
#include <sstream>

class TestHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;

public:
    TestHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) : m_objectMapper(objectMapper) {}

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        auto request_body_str = request->readBodyToString();
        std::cout<<"[DEBUG] request body: "<<request_body_str->c_str()<<std::endl;
        nlohmann::json request_body_json;
        std::stringstream ss;
        ss << std::string(request_body_str->c_str());
        ss >> request_body_json;

        auto name = request_body_json.at("name");
        auto age = request_body_json.at("age");
        std::cout<<"name : "<<nlohmann::to_string(name)<<" age: "<<age<<std::endl;

        message->statusCode = 1024;
        message->message = "Hello ! ";
        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

#endif 