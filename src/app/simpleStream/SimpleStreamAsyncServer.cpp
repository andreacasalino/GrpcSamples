#include <grpcpp/grpcpp.h>

#include <SimpleStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Spinner.h>
#include <AsyncServer.h>
#include <Periodic.h>

#include <unordered_map>

class ServerImpl : public srv::Server<srv::SimpleStreamService> {
public:
    ServerImpl(const std::string& server_address) 
    : srv::Server<srv::SimpleStreamService>{server_address} {
        addRPC<srv::EchoRequest, srv::EchoResponse>(
        [](const srv::EchoRequest& request, srv::EchoResponse& response){
            LOGI("Responding to", request.name());
            std::string resp = "Hi ";
            resp += request.name();
            resp += " from the server";
            response.set_payload(std::move(resp));
        },
        [](srv::EchoService::AsyncService& server,
        srv::AsyncHandlerData<srv::EchoRequest, srv::EchoResponse>& data, 
        ::grpc::ServerCompletionQueue& queue, srv::Tag* tag){
            server.RequestrespondEcho(&data.context, &data.request, &data.responder, &queue, &queue, tag);
        });
    }

    ::grpc::Status allCathegories(::grpc::ServerContext* context, const ::srv::AllCathegoriesRequest* request, ::grpc::ServerWriter< ::srv::Cathegory>* writer) final {
        for(const auto& [name, _] : data_) {
            srv::Cathegory msg;
            msg.set_name(name);
            writer->Write(msg);
        }
        return ::grpc::Status::OK;
    }
    
    ::grpc::Status allPeopleInCathegory(::grpc::ServerContext* context, const ::srv::StreamRequest* request, ::grpc::ServerWriter< ::srv::Person>* writer) final {
        auto it = data_.find(request->cathegory());
        if(it == data_.end()) {
            return ::grpc::Status{::grpc::StatusCode::NOT_FOUND, "Invalid cathegory"};
        }
        for(const auto& [surname, name] : it->second) {
            srv::Person msg;
            msg.set_name(name);
            msg.set_surname(surname);
            writer->Write(msg);
        }
        return ::grpc::Status::OK;
    }

private:
    using Person = std::pair<std::string, std::string>;
    static inline const std::unordered_map<std::string, std::vector<Person>> data_ = {
        {"cantanti", {{"Pavarotti", "Luciano"}, {"Ligabue", "Luciano"}, {"Pausini", "Laura"}}},
        {"filosofi", {{"Platone", "Boh"},{"Aristotele", "Boh"}, {"Socrate", "Boh"}}},
        {"scienziati", {{"Galilei", "Galileo"} , {"DaVinci", "Leonardo"}, {"Einstein", "Albert"} }}
    };
};

int main() {
    // ServerImpl service;

    // std::string server_address = carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT");
    // ::grpc::ServerBuilder builder;
    // builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    // builder.RegisterService(&service);
    // std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    // LOGI("Listening at port:", server_address);
    // server->Wait();

    return EXIT_SUCCESS;
}
