#include "controller/TestController.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "controller/Controller.hpp"
#include "FuzzerManager/FuzzerManager.hpp"

void run(){
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
    auto router = oatpp::web::server::HttpRouter::createShared();
    auto fuzzer_manager = new FuzzerManager();
    router->route("GET", "/hello", std::make_shared<TestHandler>(objectMapper ));
    router->route("POST", "/add", std::make_shared<AddFuzzerHandler>(objectMapper, fuzzer_manager));
    router->route("POST", "/pause", std::make_shared<PauseFuzzHandler>(objectMapper, fuzzer_manager));
    router->route("POST", "/resume", std::make_shared<ResumeFuzzHandler>(objectMapper, fuzzer_manager));

    /* Create HTTP connection handler with router */
    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    /* Create TCP connection provider */
    auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000, oatpp::network::Address::IP_4});

    /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    /* Priny info about server port */
    OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    /* Run server */
    server.run();
}

int main() {
    /* Init oatpp Environment */
    oatpp::base::Environment::init();
    /* Run App */
    run();

    /* Destroy oatpp Environment */
    oatpp::base::Environment::destroy();

    return 0;
}