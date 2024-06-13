#pragma once 

#include <AsyncServerBase.h>

namespace srv {
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
    , spawn_pred{spawn} {
    data.reset(new Data{});
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

}
