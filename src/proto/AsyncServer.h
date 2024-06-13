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

    template<typename RequestT, typename ResponseT, typename ProgressPredT, typename SpawnPredT>
    void addRPC(ProgressPredT&& proc, SpawnPredT&& spawn) {
        AsyncHandler<ServiceT, RequestT, ResponseT>::start(service, *queue, 
                                                           std::forward<ProgressPredT>(proc),
                                                           std::forward<SpawnPredT>(spawn),
                                                           pending_table
                                                           );
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
            Tag* as_tag = reinterpret_cast<Tag*>(got_tag);
            auto hndlr = pending_table.extract(*as_tag);
            if(hndlr) {
                hndlr->progress(pending_table);
            }
        }
    }

private:
    typename ServiceT::AsyncService service;
    std::unique_ptr<grpc::ServerCompletionQueue> queue;
    std::unique_ptr<grpc::Server> server;
    TagsTable pending_table;
};   

#define ADD_RCP(SERVER, REQUEST_T, RESPONSE_T, METHOD_NAME, LAM) \
(SERVER).addRPC<REQUEST_T, RESPONSE_T>( \
    LAM, \
    [](auto& server, auto& data, \
    ::grpc::ServerCompletionQueue& queue, srv::Tag* tag){ \
        server.METHOD_NAME(&data.context, &data.request, &data.responder, &queue, &queue, tag); \
    }); 
}
