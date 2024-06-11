#include <grpcpp/grpcpp.h>

#include <EchoService.grpc.pb.h>

class GreeterClient {
public:
  GreeterClient(std::shared_ptr<::grpc::Channel> channel)
      : stub_(srv::EchoService::NewStub(channel)) {}

  std::string request() {
    srv::FooRequest request;
    request.set_name("Pinco");
    srv::FooResponse response;
    ::grpc::ClientContext ctxt;

    ::grpc::Status status = stub_->GiveMeAFoo(&ctxt, request, &response);

    if (!status.ok()) {
      throw std::runtime_error{"Failed"};
    }
    return response.mutable_payload()->name();
  }

private:
  std::unique_ptr<srv::EchoService::Stub> stub_;
};

int main() {
  GreeterClient client{::grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials())};

  std::cout << client.request() << std::endl;

  return EXIT_SUCCESS;
}
