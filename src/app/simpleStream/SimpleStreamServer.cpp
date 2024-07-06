#include <grpcpp/grpcpp.h>
#include "ServerBase.h"
#include <SimpleStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>


class ServerImpl 
: public srv::SimpleStreamService::Service
, public srv::ServerBase {
public:
    ::grpc::Status allCathegories(::grpc::ServerContext* context, const ::srv::AllCathegoriesRequest* request, ::grpc::ServerWriter< ::srv::Cathegory>* writer) final {
        for(auto&& cathegory : this->srv::ServerBase::allCathegories()) {
            srv::Cathegory msg;
            msg.set_name(std::move(cathegory));
            writer->Write(msg);
        }
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status allPeopleInCathegory(::grpc::ServerContext* context, const ::srv::StreamRequest* request, ::grpc::ServerWriter< ::srv::Person>* writer) final {
        for(const auto& [name, surname] : this->srv::ServerBase::allPeopleInCathegory(request->cathegory())) {
            srv::Person msg;
            msg.set_name(name);
            msg.set_surname(surname);
            writer->Write(msg);
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
    auto channel = make_channel(carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT"), service);
    channel->Wait();

    return EXIT_SUCCESS;
}
