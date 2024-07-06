#include <grpcpp/grpcpp.h>
#include "ServerBase.h"
#include "../Common.h"
#include <SimpleStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>

class Server 
: public srv::AsyncServer<srv::SimpleStreamService::AsyncService>
, public srv::ServerBase {
public:
    Server(const std::string& address) 
    : srv::AsyncServer<srv::SimpleStreamService::AsyncService>{address} {

        new srv::StreamResponse<srv::AllCathegoriesRequest, srv::Cathegory>{
            [this](grpc::ServerContext& ctxt, srv::AllCathegoriesRequest& req, grpc::ServerAsyncWriter<srv::Cathegory>& resp, srv::Request* tag){
                this->RequestallCathegories(&ctxt, &req, &resp, getQueue(), getQueue(), tag);                
            },
            [this](const srv::AllCathegoriesRequest&){ return make_cathegories_stream(); }
        };

        new srv::StreamResponse<srv::StreamRequest, srv::Person>{
            [this](grpc::ServerContext& ctxt, srv::StreamRequest& req, grpc::ServerAsyncWriter<srv::Person>& resp, srv::Request* tag){
                this->RequestallPeopleInCathegory(&ctxt, &req, &resp, getQueue(), getQueue(), tag);                
            },
            std::bind(&Server::make_cathegory_stream, std::ref(*this), std::placeholders::_1)
        };

    }

private:
    class CathegoriesStream : public srv::StreamResponse<srv::AllCathegoriesRequest, srv::Cathegory>::StreamProgress {
    public:
        CathegoriesStream(std::vector<std::string>&& cat) 
        : cathegories{std::forward<std::vector<std::string>>(cat)}
        , cursor{cathegories.begin()} {}

        std::optional<srv::Cathegory> next() final {
            if(cursor == cathegories.end()) {
                return std::nullopt;
            }            
            srv::Cathegory res;
            res.set_name(*cursor);
            ++cursor;
            return res;
        }

    private:
        std::vector<std::string> cathegories;
        std::vector<std::string>::const_iterator cursor;
    };
    std::unique_ptr<CathegoriesStream> make_cathegories_stream() const {
        return std::make_unique<CathegoriesStream>(this->ServerBase::allCathegories());
    }

    class CathegoryStream : public srv::StreamResponse<srv::StreamRequest, srv::Person>::StreamProgress {
    public:
        CathegoryStream(const std::vector<srv::ServerBase::Person>& p) 
        : persons{p}
        , cursor{persons.begin()} {}

        std::optional<srv::Person> next() final {
            if(cursor == persons.end()) {
                return std::nullopt;
            }            
            srv::Person res;
            res.set_name(cursor->first);
            res.set_surname(cursor->second);
            ++cursor;
            return res;
        }

    private:
        const std::vector<srv::ServerBase::Person>& persons;
        std::vector<srv::ServerBase::Person>::const_iterator cursor;
    };
    std::unique_ptr<CathegoryStream> make_cathegory_stream(const srv::StreamRequest& req) {
        return std::make_unique<CathegoryStream>(this->ServerBase::allPeopleInCathegory(req.cathegory()));
    }
};

int main() {
    std::string server_address = carpet::getAddressFromEnv("0.0.0.0","STREAM_SERVER_PORT");

    carpet::Spinner{
        std::make_unique<Server>(server_address),
        std::make_unique<carpet::Periodic>(std::chrono::seconds{1}, [](carpet::Spinner& ){
            LOGI("============>>> Hello from the timer");
            return true;
        })
        }.run();

    return EXIT_SUCCESS;
}
