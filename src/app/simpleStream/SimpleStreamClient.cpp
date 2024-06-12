#include <grpcpp/grpcpp.h>

#include <Config.h>
#include <Logger.h>

#include <SimpleStreamService.grpc.pb.h>

class CathegoryClient {
public:
  CathegoryClient(const std::string& server_address)
      : stub_(srv::SimpleStreamService::NewStub(make_channel(server_address))) {}

  std::vector<std::string> getAllCathegories() {
    std::vector<std::string> res;
    ::grpc::ClientContext ctxt;
    auto stream = stub_->allCathegories(&ctxt, srv::AllCathegoriesRequest{});
    while (true) {
        srv::Cathegory msg;
        if(!stream->Read(&msg)) {
            break;
        }
        res.emplace_back(msg.name());
    }
    return res;
  }

  template<typename Pred>
  void ForEachPerson(const std::string& cathegory, Pred&& pred) {
    ::grpc::ClientContext ctxt;
    srv::StreamRequest request;
    request.set_cathegory(cathegory);
    auto stream = stub_->allPeopleInCathegory(&ctxt, request);
    while (true) {
        srv::Person msg;
        if(!stream->Read(&msg)) {
            break;
        }
        pred(msg.name(), msg.surname());
    }
  }

private:
  static std::shared_ptr<::grpc::Channel> make_channel(const std::string& server_address) {
    return ::grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  }

  std::unique_ptr<srv::SimpleStreamService::Stub> stub_;
};

int main() {
  std::string server_address = carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT");
  LOGI("Connecting to:", server_address);
  CathegoryClient client{server_address};

  const auto cathegories = client.getAllCathegories();
  for(const auto& cathegory : cathegories) {
    LOGI("Requesting all people under:", cathegory);
    client.ForEachPerson(cathegory, [](const std::string& name, const std::string& surname){
        LOGI("name:", name, "surname:", surname);
    });
  }

  return EXIT_SUCCESS;
}
