#pragma once 

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

namespace srv {
    class ServerBase {
    public:
        using Person = std::pair<std::string, std::string>;

        std::vector<std::string> allCathegories() const {
            std::vector<std::string> res;
            for(const auto& [k, _] : data_) {
                res.emplace_back(k);
            }
            return res;
        }

        const std::vector<Person>& allPeopleInCathegory(const std::string& cathegory) const {
            auto it = data_.find(cathegory);
            if(it == data_.end()) {
                throw std::runtime_error{"Invalid cathegory"};
            }
            return it->second;
        }

    private:
        static inline const std::unordered_map<std::string, std::vector<Person>> data_ = {
            {"cantanti", {{"Pavarotti", "Luciano"}, {"Ligabue", "Luciano"}, {"Pausini", "Laura"}}},
            {"filosofi", {{"Platone", "Boh"},{"Aristotele", "Boh"}, {"Socrate", "Boh"}}},
            {"scienziati", {{"Galilei", "Galileo"} , {"DaVinci", "Leonardo"}, {"Einstein", "Albert"} }}
        };
    };
}
