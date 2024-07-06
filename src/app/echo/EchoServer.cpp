#include <grpcpp/grpcpp.h>

#include <Config.h>
#include <Logger.h>

#include <EchoService.grpc.pb.h>

class EchoServiceImpl final : public srv::EchoService::Service {
public:
    ::grpc::Status respondEcho(::grpc::ServerContext* context, const ::srv::EchoRequest* request, ::srv::EchoResponse* response) final {
        LOGI("Responding to", request->name());
        std::string resp = "Hi ";
        resp += request->name();
        resp += " from the server";
        response->set_payload(std::move(resp));
        return ::grpc::Status::OK;
    }

    ::grpc::Status respondAnotherEcho(::grpc::ServerContext* context, const ::srv::EchoRequest* request, ::srv::EchoResponse* response) final {
        LOGI("Responding to", request->name());
        std::string resp = "Hi ";
        resp += request->name();
        resp += " from the server, but another echo ...";
        response->set_payload(std::move(resp));
        return ::grpc::Status::OK;
    }
};

std::unique_ptr<::grpc::Server> make_channel(const std::string& address, grpc::Service& implementation) {
    LOGI("Listening at:", address);
    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(address, ::grpc::InsecureServerCredentials());
    builder.RegisterService(&implementation);
    return builder.BuildAndStart();    
}

int main() {
    EchoServiceImpl service;
    auto channel = make_channel(carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT"), service);
    channel->Wait();

    return EXIT_SUCCESS;
}
