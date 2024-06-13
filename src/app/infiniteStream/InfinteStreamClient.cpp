#include <grpcpp/grpcpp.h>

#include <Config.h>
#include <Logger.h>

#include <InfiniteStreamService.grpc.pb.h>

class Client {
public:
  Client(const std::string& server_address)
      : stub_(srv::InfStreamService::NewStub(make_channel(server_address))) {}

  void consumeStream() {
    ::grpc::ClientContext ctxt;
    srv::InfStreamRequest request;
    auto stream = stub_->infStream(&ctxt, request);
    while (true) {
        srv::StreamElement msg;
        if(!stream->Read(&msg)) {
            break;
        }
        LOGI("received:", msg.name());
    }
  }

private:
  static std::shared_ptr<::grpc::Channel> make_channel(const std::string& server_address) {
    return ::grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  }

  std::unique_ptr<srv::InfStreamService::Stub> stub_;
};

int main() {
  std::string server_address = carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT");
  LOGI("Connecting to:", server_address);

  Client client{server_address};
  client.consumeStream();

  return EXIT_SUCCESS;
}
