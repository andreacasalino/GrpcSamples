#include <grpcpp/grpcpp.h>
#include "../Common.h"
#include "StreamBase.h"
#include <InfiniteStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>

class Server : public srv::AsyncServer<srv::InfStreamService::AsyncService> {
public:
    Server(const std::string& server_address)
    : srv::AsyncServer<srv::InfStreamService::AsyncService>{server_address} {

        new srv::StreamResponse<srv::InfStreamRequest, srv::StreamElement>{
            [this](grpc::ServerContext& ctxt, srv::InfStreamRequest& req, grpc::ServerAsyncWriter<srv::StreamElement>& resp, srv::Request* tag){
                this->RequestinfStream(&ctxt, &req, &resp, getQueue(), getQueue(), tag);                
            },
            [this](const srv::InfStreamRequest& ){
                return std::make_unique<InfiniteCircularStream>();
            }
        };        

    }

private:
    class InfiniteCircularStream : public srv::StreamResponse<srv::InfStreamRequest, srv::StreamElement>::StreamProgress {
    public:
        InfiniteCircularStream() = default;

        std::optional<srv::StreamElement> next() final {
            std::optional<srv::StreamElement> res;
            *res.emplace().mutable_name() = impl.nextElement();
            return res;
        }

    private:
        srv::StreamBase impl;
    };
};

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","INF_STREAM_SERVER_PORT");
    LOGI("Listening at port:", server_address);

    carpet::Spinner{
        std::make_unique<Server>(server_address),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
