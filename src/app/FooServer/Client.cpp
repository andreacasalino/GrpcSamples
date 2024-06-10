#include <grpcpp/grpcpp.h>

#include <FooService.grpc.pb.h>

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<::grpc::Channel> channel)
      : stub_(srv::FooService::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
//   std::string SayHello(const std::string& user) {
//     // Data we are sending to the server.
//     HelloRequest request;
//     request.set_name(user);

//     // Container for the data we expect from the server.
//     HelloReply reply;

//     // Context for the client. It could be used to convey extra information to
//     // the server and/or tweak certain RPC behaviors.
//     ClientContext context;

//     // The actual RPC.
//     Status status = stub_->SayHello(&context, request, &reply);

//     // Act upon its status.
//     if (status.ok()) {
//       return reply.message();
//     } else {
//       std::cout << status.error_code() << ": " << status.error_message()
//                 << std::endl;
//       return "RPC failed";
//     }
//   }

 private:
  std::unique_ptr<srv::FooService::Stub> stub_;
};

int main() {
    GreeterClient client{::grpc::CreateChannel("TODO server address", grpc::InsecureChannelCredentials())};


    return EXIT_SUCCESS;
}
