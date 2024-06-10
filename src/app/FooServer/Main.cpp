#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <FooService.grpc.pb.h>

class FooServiceImpl final : public srv::FooService::Service {
public:
    ::grpc::Status GiveMeAFoo(::grpc::ServerContext* context
                              ,const ::srv::FooRequest* request
                              , ::srv::FooResponse* response) final {
        auto* payload = response->mutable_payload();
        payload->set_name("Here is a name for you");
        return ::grpc::Status::OK;
    }
};

int main() {
    std::string server_address("0.0.0.0:50051");
    FooServiceImpl service;

    ::grpc::ServerBuilder builder;
    // builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    // builder.RegisterService(&service);
    // std::unique_ptr<Server> server(builder.BuildAndStart());
    // std::cout << "Server listening on " << server_address << std::endl;
    // server->Wait();

    return EXIT_SUCCESS;
}
