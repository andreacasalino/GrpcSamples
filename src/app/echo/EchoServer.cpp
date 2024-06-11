#include <grpcpp/grpcpp.h>

#include <Config.h>

#include <EchoService.grpc.pb.h>


class EchoServiceImpl final : public srv::EchoService::Service {
public:
    ::grpc::Status respondEcho(::grpc::ServerContext* context, const ::srv::EchoRequest* request, ::srv::EchoResponse* response) final {
        std::string resp = "Hi ";
        resp += request->name();
        resp += " from the server";
        response->set_payload(std::move(resp));
        return ::grpc::Status::OK;
    }
};

int main() {
    EchoServiceImpl service;

    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT");
    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

    return EXIT_SUCCESS;
}
