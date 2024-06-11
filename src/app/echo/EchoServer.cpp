#include <grpcpp/grpcpp.h>

#include <FooService.grpc.pb.h>

class FooServiceImpl final : public srv::FooService::Service {
public:
    ::grpc::Status GiveMeAFoo(::grpc::ServerContext* context
                              ,const ::srv::FooRequest* request
                              , ::srv::FooResponse* response) final {
        auto* payload = response->mutable_payload();
        std::string resp = "Hi ";
        resp += request->name();
        resp += " from the server";
        payload->set_name(std::move(resp));
        return ::grpc::Status::OK;
    }
};

int main() {
    FooServiceImpl service;

    std::string server_address{"0.0.0.0:50051"};
    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

    return EXIT_SUCCESS;
}
