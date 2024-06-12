#include <grpcpp/grpcpp.h>

#include <EchoService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Pollable.h>
#include <Periodic.h>

#include <optional>
#include <unordered_map>

struct Tag {};

class RPCProgress {
public:
    template<typename Process>
    static std::pair<Tag*, std::function<Tag*()>> make(srv::EchoService::AsyncService& s, ::grpc::ServerCompletionQueue& q, Process&& pred){
        std::shared_ptr<RPCProgress> progress;
        progress.reset(new RPCProgress{s, q, std::forward<Process>(pred)});
        Tag* start_tag = progress->tag.get();
        return std::make_pair(start_tag, [progress = progress](){
            return progress->progress();
        });
    }

    Tag* progress() {
        if(wait_new_or_finalize) {
            srv::EchoResponse reply = process_pred(data->request);
            data->responder.Finish(reply, ::grpc::Status::OK, generateTag());
        }
        else {
            beginNewRequest();
        }
        wait_new_or_finalize = !wait_new_or_finalize;
        return tag.get();
    }

private:
    template<typename Process>
    RPCProgress(srv::EchoService::AsyncService& s, ::grpc::ServerCompletionQueue& q, Process&& pred) 
        : server{s}, queue{q}, process_pred{std::forward<Process>(pred)} {
        if(!process_pred) {
            THROW_ERROR("Invalid process predicate");
        }
        beginNewRequest();
    }

    void beginNewRequest() {
        data.emplace(context);
        server.RequestrespondEcho(&context, &data->request, &data->responder, &queue, &queue, generateTag());
    }

    srv::EchoService::AsyncService& server;
    ::grpc::ServerCompletionQueue& queue;
    ::grpc::ServerContext context;
    std::function<srv::EchoResponse(const srv::EchoRequest& )> process_pred;

    bool wait_new_or_finalize = true;
    std::unique_ptr<Tag> tag;
    Tag* generateTag() { 
        tag.reset(new Tag{}); 
        return tag.get();
    }

    struct Data {
        Data(::grpc::ServerContext& context) : responder{&context} {}

        srv::EchoRequest request;
        ::grpc::ServerAsyncResponseWriter<srv::EchoResponse> responder;
    };
    std::optional<Data> data;
};

class Server : public carpet::Pollable {
public:
    Server(const std::string& server_address) {
        ::grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        queue = builder.AddCompletionQueue();
        server = builder.BuildAndStart();
        //set up services
        auto [tag, fnct] = RPCProgress::make(service, *queue, [this](const srv::EchoRequest& req){
            return handle(req);
        });
        requests.emplace(tag, std::move(fnct));
    }

    ~Server() {
        server->Shutdown();
        queue->Shutdown();
    }

    bool poll(carpet::Spinner&) override {
        void* got_tag = nullptr;
        bool ok = false;
        auto deadline = std::chrono::high_resolution_clock::now();
        deadline += std::chrono::milliseconds{50};
        auto next_status = queue->AsyncNext(&got_tag, &ok, deadline);
        if(next_status == ::grpc::CompletionQueue::NextStatus::GOT_EVENT && ok) {
            auto it = requests.find(reinterpret_cast<Tag*>(got_tag));
            if(it != requests.end()) {
                Tag* new_tag = it->second();
                requests.emplace(new_tag, std::move(it->second));
                requests.erase(reinterpret_cast<Tag*>(got_tag));
            }
        }
        return true;
    }

private:
    srv::EchoResponse handle(const srv::EchoRequest& request) {
        LOGI("Responding to", request.name());
        srv::EchoResponse response;
        std::string resp = "Hi ";
        resp += request.name();
        resp += " from the server";
        response.set_payload(std::move(resp));
        return response;
    }

    srv::EchoService::AsyncService service;
    std::unique_ptr<grpc::ServerCompletionQueue> queue;
    std::unique_ptr<grpc::Server> server;
    std::unordered_map< Tag*, std::function<Tag*()> > requests;
};

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","ECHO_SERVER_PORT");

    carpet::Spinner{
        std::make_unique<Server>(server_address),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
