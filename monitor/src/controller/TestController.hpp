#ifndef TEST_CONTROLLER_HPP
#define TEST_CONTROLLER_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/networkServer.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

class TestHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;

public:
    TestHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper) : m_objectMapper(objectMapper) {}

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();
        message->statusCode = 1024;
        message->message = "Hello ! ";
        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
}

#endif 