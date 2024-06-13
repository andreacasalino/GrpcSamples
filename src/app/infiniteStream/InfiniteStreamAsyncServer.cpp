#include <grpcpp/grpcpp.h>

#include <InfiniteStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>
#include <AsyncServer.h>

#include <unordered_map>

class ServerImpl : public srv::Server<srv::InfStreamService> {
public:
    ServerImpl(const std::string& server_address) 
    : srv::Server<srv::InfStreamService>{server_address} {
        ADD_STREAM_RCP(*this, srv::InfStreamRequest, srv::StreamElement, 
                       RequestinfStream, [](const srv::InfStreamRequest& req){
                        return std::make_unique<InfiniteStream>(req);
                       });
    }

private:
    class InfiniteStream : public srv::StreamGenerator<srv::InfStreamRequest, srv::StreamElement> {
    public:
        InfiniteStream(const srv::InfStreamRequest& r) : srv::StreamGenerator<srv::InfStreamRequest, srv::StreamElement>{r}
                                                       , names{"Foo", "Bla", "Dummy", "Fuffa"}, cursor{names.begin()} {
            LOGI("============>>> starting new stream");
        }

        std::optional<srv::StreamElement> next() override {
            if(cursor == names.end()) {
                cursor = names.begin();
            }            
            srv::StreamElement res;
            res.set_name((*cursor));
            ++cursor;
            return res;
        }

    private:
        std::vector<std::string> names;
        std::vector<std::string>::const_iterator cursor;
    };
};

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT");
    LOGI("Listening at port:", server_address);

    carpet::Spinner{
        std::make_unique<carpet::PredicatePollable>([server = std::make_shared<ServerImpl>(server_address)](carpet::Spinner&) {
            server->poll();
            return true;
        }),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
