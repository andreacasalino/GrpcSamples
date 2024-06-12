#pragma once 

#include <grpcpp/grpcpp.h>

#include <functional>
#include <memory>
#include <optional>

namespace srv {

struct Tag {};

template<typename RequestT, typename ResponseT>
struct AsyncHandlerData {
    ::grpc::ServerContext context;
    RequestT request;
    ::grpc::ServerAsyncResponseWriter<ResponseT> responder{&context};
};

template<typename RequestT, typename ResponseT>
using ProcessPred = std::function<void(const RequestT&, ResponseT&)>;

template<typename ServiceT, typename RequestT, typename ResponseT>
using BeginReceivePred = std::function<void(typename ServiceT::AsyncService&, AsyncHandlerData<RequestT, ResponseT>&, ::grpc::ServerCompletionQueue&, Tag*)>;

template<typename ServiceT, typename RequestT, typename ResponseT>
class AsyncHandler {
public:
    Tag* progress();

    template<typename ProcessPredT, typename BeginReceivePredT>
    std::pair<Tag*, std::function<Tag*()>> make(ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q, 
                                            ProcessPredT&& proc, BeginReceivePredT&& begin) {
        using Handler = AsyncHandler<ServiceT, RequestT, ResponseT>;
        std::shared_ptr<Handler> progress;
        progress.reset(new Handler{s, q, std::forward<ProcessPredT>(proc), std::forward<BeginReceivePredT>(begin)});
        Tag* start_tag = progress->tag.get();
        return std::make_pair(start_tag, [progress = progress](){
            return progress->progress();
        });
    }

private:
    template<typename ProcessPredT, typename BeginReceivePredT>
    AsyncHandler(typename ServiceT::AsyncService& s, ::grpc::ServerCompletionQueue& q,
                 ProcessPredT&& proc, BeginReceivePredT&& begin) 
        : server{s}, queue{q}
        , process_pred{std::forward<ProcessPredT>(proc)}
        , begin_new_pred{std::forward<BeginReceivePredT>(begin)} {
        if(!process_pred) {
            THROW_ERROR("Invalid process predicate");
        }
        beginNewRequest();
    }

    using Data = AsyncHandlerData<RequestT, ResponseT>;
    
    void beginNewRequest();

    typename ServiceT::AsyncService& server;
    ::grpc::ServerCompletionQueue& queue;
    ProcessPred<RequestT, ResponseT> process_pred;
    BeginReceivePred<ServiceT, RequestT, ResponseT> begin_new_pred;

    bool wait_new_or_finalize = true;
    std::unique_ptr<Tag> tag;
    Tag* generateTag();

    std::optional<Data> data;
};

///////////////////////////////////////////////////////////////////////

template<typename ServiceT, typename RequestT, typename ResponseT>
Tag* AsyncHandler<ServiceT, RequestT, ResponseT>::progress() {
    if(wait_new_or_finalize) {
        ResponseT reply;
        process_pred(reply, data->request);
        data->responder.Finish(std::move(reply), ::grpc::Status::OK, generateTag());
    }
    else {
        beginNewRequest();
    }
    wait_new_or_finalize = !wait_new_or_finalize;
    return tag.get();
}

template<typename ServiceT, typename RequestT, typename ResponseT>
void AsyncHandler<ServiceT, RequestT, ResponseT>::beginNewRequest() {
    data.emplace();
    // server.RequestrespondEcho(&data->context, &data->request, &data->responder, &queue, &queue, generateTag());
    begin_new_pred(server, data.value(), queue, generateTag());
}

template<typename ServiceT, typename RequestT, typename ResponseT>
Tag* AsyncHandler<ServiceT, RequestT, ResponseT>::generateTag() { 
    tag.reset(new Tag{}); 
    return tag.get();
}
}
