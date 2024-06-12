#include <grpcpp/grpcpp.h>

#include <SimpleStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>

class RequestGuard {
public:
RequestGuard() = default;

private:
::grpc::ServerContext context;
struct Data {
srv::Echo
HelloRequest request;
ServerAsyncResponseWriter<HelloReply> responder;    
};

};
