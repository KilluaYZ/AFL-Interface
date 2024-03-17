#ifndef TEST_CONTROLLER_HPP
#define TEST_CONTROLLER_HPP

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "../dto/MsgDto.hpp"
#include "../utils/case_info.hpp"
#include "../FuzzerManager/FuzzerManager.hpp"

class PauseHandler : public oatpp::web::server::HttpRequestHandler {
private:
    std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
    FuzzerManager *m_fuzzerManager;

public:
    PauseHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper, FuzzerManager* fuzzer_manager) : m_objectMapper(objectMapper) {
        m_fuzzerManager = fuzzer_manager;
    }

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
        auto message = MessageDto::createShared();

        try{
            m_fuzzerManager->get_fuzzer()->pause();
            message->statusCode = 200;
            message->message = "pause success";
        }catch(std::runtime_error& e){
            std::cerr<<e.what()<<std::endl;
            message->statusCode = 500;
            message->message = "pause failed: "+e.what();
        }

        return ResponseFactory::createResponse(Status::CODE_200, message, m_objectMapper);
    }
};

#endif 