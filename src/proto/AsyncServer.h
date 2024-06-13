#pragma once 

#include <grpcpp/grpcpp.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace srv {
struct Tag {
    static std::unique_ptr<Tag> make() { return std::make_unique<Tag>(); }
};

class IAsyncHandler;
using IAsyncHandlerPtr = std::unique_ptr<IAsyncHandler>;

class TagsTable {
public:
TagsTable() = default;

IAsyncHandlerPtr extract(Tag& tag);

void emplace(std::unique_ptr<Tag> tag, IAsyncHandlerPtr handler);

private:
std::unordered_map<Tag*, std::pair< IAsyncHandlerPtr , std::unique_ptr<Tag> >> table;
};

class IAsyncHandler {
public:
virtual void progress(TagsTable& table) = 0;
};

template<typename ServiceT, typename RequestT, typename ResponseT>
class AsyncHandler : public IAsyncHandler {
public:
    struct Data {
        ::grpc::ServerContext context;
        RequestT request;
        ::grpc::ServerAsyncResponseWriter<ResponseT> responder{&context};
    };
    using ProgressPred = std::function<void(const RequestT&, ResponseT&)>;
    using SpawnPred = std::function<void(typename ServiceT::AsyncService&, Data&, ::grpc::ServerCompletionQueue&, Tag*)>;

    template<typename ProgressPredT, typename SpawnPredT>
    static void start(ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q, 
                     ProgressPredT&& proc, SpawnPredT&& spawn, TagsTable& table);

    void progress(TagsTable& table) override;                                  

private:
    AsyncHandler(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q,
                 std::shared_ptr<ProgressPred> proc, std::shared_ptr<SpawnPred> spawn);

    AsyncHandler(const AsyncHandler& );
    AsyncHandler* operator=(const AsyncHandler& ) = delete;

    AsyncHandler(AsyncHandler&& ) = default;
    AsyncHandler* operator=(AsyncHandler&& ) = delete;

    void spawn(TagsTable& table);

    typename ServiceT::AsyncService& server;
    ::grpc::ServerCompletionQueue& queue;
    std::shared_ptr<ProgressPred> process_pred;
    std::shared_ptr<SpawnPred> spawn_pred;
    std::unique_ptr<Data> data;
    bool wait_message_reception = true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ServiceT, typename RequestT, typename ResponseT>
AsyncHandler<ServiceT, RequestT, ResponseT>::AsyncHandler(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q,
                                                          std::shared_ptr<ProgressPred> proc, std::shared_ptr<SpawnPred> spawn) 
    : server{s}, queue{q}
    , process_pred{proc}
    , spawn_pred{spawn}
    , data{std::make_unique<Data>()} {
    if(!(*process_pred) || !(*spawn_pred)) {
        throw std::runtime_error("Invalid predicates");
    }
}

template<typename ServiceT, typename RequestT, typename ResponseT>
AsyncHandler<ServiceT, RequestT, ResponseT>::AsyncHandler(const AsyncHandler& o)
    : AsyncHandler{o.server, o.queue, o.process_pred, o.spawn_pred} {}

template<typename ServiceT, typename RequestT, typename ResponseT>
template<typename ProgressPredT, typename SpawnPredT>
void AsyncHandler<ServiceT, RequestT, ResponseT>::start(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q, 
                                                        ProgressPredT&& proc, SpawnPredT&& spawn, TagsTable& table) {
    auto proc_ptr = std::make_shared<ProgressPred>(std::forward<ProgressPredT>(proc));
    auto spawn_ptr = std::make_shared<SpawnPred>(std::forward<SpawnPredT>(spawn));
    AsyncHandler<ServiceT, RequestT, ResponseT>{s,q,proc_ptr,spawn_ptr}.spawn(table);
}

template<typename ServiceT, typename RequestT, typename ResponseT>
void AsyncHandler<ServiceT, RequestT, ResponseT>::progress(TagsTable& table) {
    if(wait_message_reception) {
        spawn(table);
        auto new_tag = Tag::make();
        ResponseT reply;
        (*process_pred)(data->request, reply);
        data->responder.Finish(std::move(reply), ::grpc::Status::OK, new_tag.get());
        std::unique_ptr<AsyncHandler<ServiceT, RequestT, ResponseT>> new_hndlr;
        new_hndlr.reset(new AsyncHandler<ServiceT, RequestT, ResponseT>{std::move(*this)});
        new_hndlr->wait_message_reception = false;
        table.emplace(std::move(new_tag), std::move(new_hndlr));
    }
    else {
        // TODO clean up if needed
    }
}

template<typename ServiceT, typename RequestT, typename ResponseT>
void AsyncHandler<ServiceT, RequestT, ResponseT>::spawn(TagsTable& table) {
    std::unique_ptr<AsyncHandler<ServiceT, RequestT, ResponseT>> new_hndlr;
    new_hndlr.reset(new AsyncHandler<ServiceT, RequestT, ResponseT>{*this});
    auto new_tag = Tag::make();
    (*spawn_pred)(server, *new_hndlr->data, queue, new_tag.get());
    table.emplace(std::move(new_tag), std::move(new_hndlr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
