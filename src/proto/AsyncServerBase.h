#pragma once 

#include <grpcpp/grpcpp.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace srv {
struct Tag {
    static std::unique_ptr<Tag> make() { return std::make_unique<Tag>(); }
};

class IAsyncHandler;
using IAsyncHandlerPtr = std::unique_ptr<IAsyncHandler>;

class TagsTable {
public:
TagsTable() = default;

IAsyncHandlerPtr extract(Tag& tag);

void emplace(std::unique_ptr<Tag> tag, IAsyncHandlerPtr handler);

private:
std::unordered_map<Tag*, std::pair< IAsyncHandlerPtr , std::unique_ptr<Tag> >> table;
};

class IAsyncHandler {
public:
virtual ~IAsyncHandler() = default;
virtual void progress(TagsTable& table) = 0;
}; 
}
