#include <AsyncServer.h>

namespace srv {
IAsyncHandlerPtr TagsTable::extract(Tag& tag) {
    auto it = table.find(&tag);
    if(it == table.end()) {
        return nullptr;
    }
    IAsyncHandlerPtr retVal = std::move(it->second.first);
    table.erase(it);
    return retVal;
}

void TagsTable::emplace(std::unique_ptr<Tag> tag, IAsyncHandlerPtr handler) {
    auto* t = tag.get();
    table.emplace(t, std::make_pair(std::move(handler), std::move(tag)) );
}
}
