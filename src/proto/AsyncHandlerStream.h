#pragma once 

#include <AsyncServerBase.h>

namespace srv {
template<typename RequestT, typename ResponseT>
class StreamGenerator {
public:
    StreamGenerator(const RequestT& request): request{request} {}

    virtual ~StreamGenerator() = default;
    virtual std::optional<ResponseT> next() = 0;

    const auto& getRequest() const { return request; }

private:
    const RequestT& request;
};

template<typename ServiceT, typename RequestT, typename ResponseT>
class AsyncHandlerStream : public IAsyncHandler {
public:
    struct Data {
        ::grpc::ServerContext context;
        RequestT request;
        ::grpc::ServerAsyncWriter<ResponseT> responder{&context};
    };
    using StreamGeneratorFactoryPred = std::function<std::unique_ptr<StreamGenerator<RequestT, ResponseT>>(const RequestT&)>;
    using SpawnPred = std::function<void(typename ServiceT::AsyncService&, Data&, ::grpc::ServerCompletionQueue&, Tag*)>;

    template<typename StreamGeneratorFactoryPredT, typename SpawnPredT>
    static void start(ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q, 
                     StreamGeneratorFactoryPredT&& proc, SpawnPredT&& spawn, TagsTable& table);

    void progress(TagsTable& table) override;                                  

private:
    AsyncHandlerStream(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q,
                       std::shared_ptr<StreamGeneratorFactoryPred> proc, std::shared_ptr<SpawnPred> spawn);

    AsyncHandlerStream(const AsyncHandlerStream& );
    AsyncHandlerStream* operator=(const AsyncHandlerStream& ) = delete;

    AsyncHandlerStream(AsyncHandlerStream&& ) = default;
    AsyncHandlerStream* operator=(AsyncHandlerStream&& ) = delete;

    void spawn(TagsTable& table);

    typename ServiceT::AsyncService& server;
    ::grpc::ServerCompletionQueue& queue;
    std::shared_ptr<StreamGeneratorFactoryPred> gen_pred;
    std::shared_ptr<SpawnPred> spawn_pred;
    std::unique_ptr<Data> data;
    std::unique_ptr<StreamGenerator<RequestT, ResponseT>> stream_gen;
    bool wait_start_stream = true;
    bool wait_finalize = false;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ServiceT, typename RequestT, typename ResponseT>
AsyncHandlerStream<ServiceT, RequestT, ResponseT>::AsyncHandlerStream(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q,
                                                                      std::shared_ptr<StreamGeneratorFactoryPred> proc, std::shared_ptr<SpawnPred> spawn) 
    : server{s}, queue{q}
    , gen_pred{proc}
    , spawn_pred{spawn} {
    data.reset(new Data{});
    if(!(*gen_pred) || !(*spawn_pred)) {
        throw std::runtime_error("Invalid predicates");
    }
}

template<typename ServiceT, typename RequestT, typename ResponseT>
AsyncHandlerStream<ServiceT, RequestT, ResponseT>::AsyncHandlerStream(const AsyncHandlerStream& o)
    : AsyncHandlerStream{o.server, o.queue, o.gen_pred, o.spawn_pred} {}

template<typename ServiceT, typename RequestT, typename ResponseT>
template<typename StreamGeneratorFactoryPredT, typename SpawnPredT>
void AsyncHandlerStream<ServiceT, RequestT, ResponseT>::start(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q, 
                                                        StreamGeneratorFactoryPredT&& proc, SpawnPredT&& spawn, TagsTable& table) {
    auto proc_ptr = std::make_shared<StreamGeneratorFactoryPred>(std::forward<StreamGeneratorFactoryPredT>(proc));
    auto spawn_ptr = std::make_shared<SpawnPred>(std::forward<SpawnPredT>(spawn));
    AsyncHandlerStream<ServiceT, RequestT, ResponseT>{s,q,proc_ptr,spawn_ptr}.spawn(table);
}

template<typename ServiceT, typename RequestT, typename ResponseT>
void AsyncHandlerStream<ServiceT, RequestT, ResponseT>::progress(TagsTable& table) {
    if(wait_start_stream) {
        spawn(table);
        stream_gen = (*gen_pred)(data->request);
        wait_start_stream = false;
    }
    if(wait_finalize) {
        // TODO clean up if needed
        return;
    }
    std::optional<ResponseT> next_msg = stream_gen->next();
    auto new_tag = Tag::make();
    using Handler =  AsyncHandlerStream<ServiceT, RequestT, ResponseT>;
    std::unique_ptr<Handler> new_hndlr;
    new_hndlr.reset(new Handler{std::move(*this)});
    if(next_msg.has_value()) {
        new_hndlr->data->responder.Write(next_msg.value(), new_tag.get());
    }
    else {
        new_hndlr->data->responder.Finish(::grpc::Status::OK, new_tag.get());
        new_hndlr->wait_finalize = true;
    }
    table.emplace(std::move(new_tag), std::move(new_hndlr));
}

template<typename ServiceT, typename RequestT, typename ResponseT>
void AsyncHandlerStream<ServiceT, RequestT, ResponseT>::spawn(TagsTable& table) {
    using Handler =  AsyncHandlerStream<ServiceT, RequestT, ResponseT>;
    std::unique_ptr<Handler> new_hndlr;
    new_hndlr.reset(new Handler{*this});
    auto new_tag = Tag::make();
    (*spawn_pred)(server, *new_hndlr->data, queue, new_tag.get());
    table.emplace(std::move(new_tag), std::move(new_hndlr));
}

}
