#pragma once 

#include <Logger.h>

#include <vector>
#include <string>

namespace srv {
    class StreamBase {
    public:
        StreamBase() {
            LOGI("============>>> starting new stream");
        }

        const std::string& nextElement() {
            if(cursor == getNames().end()) {
                cursor = getNames().begin();
            }            
            const std::string& res = *cursor;
            ++cursor;
            return res;
        }

    private:
        static const std::vector<std::string>& getNames() {
            static std::vector<std::string> res = std::vector<std::string>{"Foo", "Bla", "Hello", "Adios"};
            return res;
        }
        std::vector<std::string>::const_iterator cursor = getNames().begin();
    };
}
