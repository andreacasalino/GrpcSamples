#include <grpcpp/grpcpp.h>
#include "../Common.h"
#include <EchoService.grpc.pb.h>

#include <Config.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>

class Server : public srv::AsyncServer<srv::EchoService::AsyncService> {
public:
    Server(const std::string& address) 
    : srv::AsyncServer<srv::EchoService::AsyncService>{address} {

        new srv::OneShotRequest<srv::EchoRequest, srv::EchoResponse>{
            [this](grpc::ServerContext& ctxt, srv::EchoRequest& req, grpc::ServerAsyncResponseWriter<srv::EchoResponse>& resp, srv::Request* tag){
                this->RequestrespondEcho(&ctxt, &req, &resp, getQueue(), getQueue(), tag);                
            },
            std::bind(&Server::processEcho, std::ref(*this), std::placeholders::_1)
        };

        new srv::OneShotRequest<srv::EchoRequest, srv::EchoResponse>{
            [this](grpc::ServerContext& ctxt, srv::EchoRequest& req, grpc::ServerAsyncResponseWriter<srv::EchoResponse>& resp, srv::Request* tag){
                this->RequestrespondAnotherEcho(&ctxt, &req, &resp, getQueue(), getQueue(), tag);                
            },
            std::bind(&Server::processAnotherEcho, std::ref(*this), std::placeholders::_1)
        };

    }

private:
    srv::EchoResponse processEcho(const srv::EchoRequest& request) {
        LOGI("Responding to", request.name());
        std::string resp = "Hi ";
        resp += request.name();
        resp += " from the server";
        srv::EchoResponse response;
        response.set_payload(std::move(resp));
        return response;
    }
    srv::EchoResponse processAnotherEcho(const srv::EchoRequest& request) {
        LOGI("Responding to", request.name());
        std::string resp = "Hi ";
        resp += request.name();
        resp += " from the server, but another echo ...";
        srv::EchoResponse response;
        response.set_payload(std::move(resp));
        return response;
    }
};

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT");

    carpet::Spinner{
        std::make_unique<Server>(server_address),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
