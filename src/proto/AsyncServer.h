#pragma once 

#include <AsyncHandler.h>

namespace srv {
template<typename ServiceT>
class Server {
public:
    Server(const std::string& server_address) {
        ::grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        queue = builder.AddCompletionQueue();
        server = builder.BuildAndStart();
    }

    template<typename RequestT, typename ResponseT, typename ProcessPredT, typename BeginReceivePredT>
    void addRPC(ProcessPredT&& proc, BeginReceivePredT&& begin) {
        auto [tag, fnct] = AsyncHandler<ServiceT, RequestT, ResponseT>::make(service, *queue, 
                                                                             std::forward<ProcessPredT>(proc),
                                                                             std::forward<BeginReceivePredT>(begin));
        requests.emplace(tag, std::move(fnct));
    }

    ~Server() {
        server->Shutdown();
        queue->Shutdown();
    }

    void poll() {
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
    }

private:
    typename ServiceT::AsyncService service;
    std::unique_ptr<grpc::ServerCompletionQueue> queue;
    std::unique_ptr<grpc::Server> server;
    std::unordered_map< Tag*, std::function<Tag*()> > requests;
};    
}
