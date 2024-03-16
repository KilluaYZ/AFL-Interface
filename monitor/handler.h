#ifndef HANDLER_H
#define HANDLER_H

#include "oatpp/web/server/HttpRequestHandler.hpp"

#define O_UNUSED(x) (void)x;

class Handler : public oatpp::web::server::HttpRequestHandler {
    public:
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override{
        O_UNUSED(request);
        return ResponseFactory::createResponse(Status::CODE_200, "Hello, World!");
    }
};

#endif 