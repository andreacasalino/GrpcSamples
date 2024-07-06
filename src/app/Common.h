#pragma once 

#include <grpcpp/grpcpp.h>

#include <Spinner.h>
#include <Logger.h>

#include <functional>
#include <optional>

namespace srv {

class Request {
public:
    virtual ~Request() = default;
    virtual void progress() = 0;
};

template<typename GrpcAsyncServiceT>
class AsyncServer
: public GrpcAsyncServiceT
, public carpet::Pollable {
public:
    AsyncServer(const std::string& address) {
        LOGI("Listening at:", address);
        grpc::ServerBuilder builder;
        builder.AddListeningPort(address, ::grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        queue = builder.AddCompletionQueue();
        channel = builder.BuildAndStart();   
    }

    ~AsyncServer() {
        queue->Shutdown();
    }

    bool poll(carpet::Spinner &) override {
        void* tag;
        bool ok;
        auto status = queue->AsyncNext(&tag, &ok, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds{5});
        if(status == grpc::CompletionQueue::NextStatus::SHUTDOWN) {
            return false;
        }
        if(status == grpc::CompletionQueue::NextStatus::GOT_EVENT && ok) {
            static_cast<Request*>(tag)->progress();            
        }
        return true;
    }

protected:
    grpc::ServerCompletionQueue* getQueue() { return queue.get(); }

private:
    std::unique_ptr<grpc::ServerCompletionQueue> queue;
    std::unique_ptr<grpc::Server> channel;
};

template<typename RequestT, typename ResponseT>
class OneShotRequest : public Request {
public:
    using InitPred = std::function<void(grpc::ServerContext&, RequestT&, grpc::ServerAsyncResponseWriter<ResponseT>&, Request*)>;
    using ProgressPred = std::function<ResponseT(const RequestT&)>;

    template<typename InitPredT, typename ProgressPredT>
    OneShotRequest(InitPredT init, ProgressPredT progress) 
    : init_pred{std::move(init)}
    , progress_pred{std::move(progress)} {
        init_pred(ctxt, request, response_writer, this);
    }

    void progress() final {
        if(waitDone) {
            // TODO clean up
            delete this;
            return;
        }
        new OneShotRequest{init_pred, progress_pred}; // spawn a new request
        try {
            auto resp = progress_pred(request);        
            response_writer.Finish(std::move(resp), grpc::Status::OK, this);
        }
        catch(const std::exception& e) {
            response_writer.FinishWithError(grpc::Status{grpc::StatusCode::INVALID_ARGUMENT, e.what()} , this);
        }
        waitDone = true;
    }

private:
    InitPred init_pred;
    ProgressPred progress_pred;

    bool waitDone = false;
    grpc::ServerContext ctxt;
    RequestT request;
    grpc::ServerAsyncResponseWriter<ResponseT> response_writer{&ctxt};
};

template<typename RequestT, typename ResponseT>
class StreamResponse : public Request {
public:
    using InitPred = std::function<void(grpc::ServerContext&, RequestT&, grpc::ServerAsyncWriter<ResponseT>&, Request*)>;

    class StreamProgress {
    public:
        virtual std::optional<ResponseT> next() = 0; 
    };
    using StreamProgressPtr = std::unique_ptr<StreamProgress>;
    using ProgressMakePred = std::function<StreamProgressPtr(const RequestT&)>;

    template<typename InitPredT, typename ProgressMakePredT>
    StreamResponse(InitPredT init, ProgressMakePredT factory) 
    : init_pred{std::move(init)}
    , factory_pred{std::move(factory)} {
        init_pred(ctxt, request, stream_writer, this);
    }

    void progress() final {
        if(waitDone) {
            // TODO clean up
            delete this;
            return;
        }
        try {
            if(progressImpl == nullptr) {
                new StreamResponse{init_pred, factory_pred}; // spawn a new request
                progressImpl = factory_pred(request);
            }
            if(auto resp = progressImpl->next(); resp.has_value()) {
                stream_writer.Write(resp.value(), this);
            }      
            else {
                // finalize gracefully
                stream_writer.Finish(grpc::Status::OK, this);
                waitDone = true;
            }  
        }
        catch(const std::exception& e) {
            stream_writer.Finish(grpc::Status{grpc::StatusCode::INVALID_ARGUMENT, e.what()}, this);
            waitDone = true;
        }
    }

private:
    InitPred init_pred;
    ProgressMakePred factory_pred;

    bool waitDone = false;
    grpc::ServerContext ctxt;
    RequestT request;
    grpc::ServerAsyncWriter<ResponseT> stream_writer{&ctxt};
    StreamProgressPtr progressImpl;
};

}
