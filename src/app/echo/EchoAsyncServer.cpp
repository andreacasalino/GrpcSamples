#include <grpcpp/grpcpp.h>

#include <EchoService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Pollable.h>
#include <Periodic.h>
#include <AsyncServer.h>

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT");

    using Server = srv::Server<srv::EchoService>;
    auto server = std::make_shared<Server>(server_address);

    ADD_RCP(*server, srv::EchoRequest, srv::EchoResponse, RequestrespondEcho,
    [](const srv::EchoRequest& request, srv::EchoResponse& response){
        LOGI("Responding to", request.name());
        std::string resp = "Hi ";
        resp += request.name();
        resp += " from the server";
        response.set_payload(std::move(resp));
    });

    ADD_RCP(*server, srv::EchoRequest, srv::EchoResponse, RequestrespondAnotherEcho,
    [](const srv::EchoRequest& request, srv::EchoResponse& response){
        LOGI("Responding to", request.name());
        std::string resp = "Hi ";
        resp += request.name();
        resp += " from the server, but another echo ...";
        response.set_payload(std::move(resp));
    });

    carpet::Spinner{
        std::make_unique<carpet::PredicatePollable>([server = server](carpet::Spinner&) {
            server->poll();
            return true;
        }),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
