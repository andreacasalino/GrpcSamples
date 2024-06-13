#include <grpcpp/grpcpp.h>

#include <SimpleStreamService.grpc.pb.h>

#include <Config.h>
#include <Logger.h>
#include <Error.h>
#include <Spinner.h>
#include <Periodic.h>
#include <AsyncServer.h>

#include <unordered_map>

class ServerImpl : public srv::Server<srv::SimpleStreamService> {
public:
    ServerImpl(const std::string& server_address) 
    : srv::Server<srv::SimpleStreamService>{server_address} {
        ADD_STREAM_RCP(*this, srv::AllCathegoriesRequest, srv::Cathegory, 
                       RequestallCathegories, [&data = this->data](const srv::AllCathegoriesRequest& req){
                        return std::make_unique<CathegoriesStream>(req, data);
                       });
        ADD_STREAM_RCP(*this, srv::StreamRequest, srv::Person, 
                       RequestallPeopleInCathegory, [&data = this->data](const srv::StreamRequest& req){
                        return std::make_unique<CathegoryStream>(req, data.at(req.cathegory()));
                       });
    }

private:
    using Person = std::pair<std::string, std::string>;
    using Data = std::unordered_map<std::string, std::vector<Person>>;

    class CathegoriesStream : public srv::StreamGenerator<srv::AllCathegoriesRequest, srv::Cathegory> {
    public:
        CathegoriesStream(const srv::AllCathegoriesRequest& req, const Data& data) 
        : srv::StreamGenerator<srv::AllCathegoriesRequest, srv::Cathegory>{req}, source{data}, cursor{source.begin()} {
        }

        std::optional<srv::Cathegory> next() override {
            if(cursor == source.end()) {
                return std::nullopt;
            }            
            srv::Cathegory res;
            res.set_name(cursor->first);
            ++cursor;
            return res;
        }

    private:
        const Data& source;
        Data::const_iterator cursor;
    };

    class CathegoryStream : public srv::StreamGenerator<srv::StreamRequest, srv::Person> {
    public:
        CathegoryStream(const srv::StreamRequest& req, const std::vector<Person>& data) 
        : srv::StreamGenerator<srv::StreamRequest, srv::Person>{req}, source{data}, cursor{source.begin()} {}

        std::optional<srv::Person> next() override {
            if(cursor == source.end()) {
                return std::nullopt;
            }            
            srv::Person res;
            res.set_name(cursor->first);
            res.set_surname(cursor->second);
            ++cursor;
            return res;
        }

    private:
        const std::vector<Person>& source;
        std::vector<Person>::const_iterator cursor;
    };

    const Data data = {
        {"cantanti", {{"Pavarotti", "Luciano"}, {"Ligabue", "Luciano"}, {"Pausini", "Laura"}}},
        {"filosofi", {{"Platone", "Boh"},{"Aristotele", "Boh"}, {"Socrate", "Boh"}}},
        {"scienziati", {{"Galilei", "Galileo"} , {"DaVinci", "Leonardo"}, {"Einstein", "Albert"} }}
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
