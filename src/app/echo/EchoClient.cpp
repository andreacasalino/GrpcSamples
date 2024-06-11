#include <grpcpp/grpcpp.h>

#include <Config.h>

#include <EchoService.grpc.pb.h>

class GreeterClient {
public:
  GreeterClient(std::shared_ptr<::grpc::Channel> channel)
      : stub_(srv::EchoService::NewStub(channel)) {}

  std::string request() {
    srv::EchoRequest request;
    request.set_name("Pinco");
    srv::EchoResponse response;
    ::grpc::ClientContext ctxt;

    ::grpc::Status status = stub_->respondEcho(&ctxt, request, &response);

    if (!status.ok()) {
      throw std::runtime_error{"Failed"};
    }
    return response.payload();
  }

private:
  std::unique_ptr<srv::EchoService::Stub> stub_;
};

int main() {
  std::string server_address = carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT");
  GreeterClient client{::grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};

  std::cout << client.request() << std::endl;

  return EXIT_SUCCESS;
}
