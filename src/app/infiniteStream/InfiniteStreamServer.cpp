#include <grpcpp/grpcpp.h>
#include "StreamBase.h"
#include <InfiniteStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>

#include <thread>

class ServerImpl : public srv::InfStreamService::Service {
public:
    ::grpc::Status infStream(::grpc::ServerContext* context, const ::srv::InfStreamRequest* request, ::grpc::ServerWriter< ::srv::StreamElement>* writer) final {
        srv::StreamBase stream; 
        while (true) {
            srv::StreamElement msg;
            msg.set_name(stream.nextElement());
            writer->Write(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
        }
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
    ServerImpl service;
    auto channel = make_channel(carpet::getAddressFromEnv("0.0.0.0","INF_STREAM_SERVER_PORT"), service);
    channel->Wait();

    return EXIT_SUCCESS;
}
