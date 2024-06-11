#pragma once

#include <OrderBookService.grpc.pb.h>

#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include <optional>

namespace book {
struct BookSide {
BookSide(bool s) : side{s} {}

bool side;

struct Info {
std::uint32_t quantity;
srv::Ids ids;
};
using Container = std::multimap<std::uint32_t, Info>;
Container orders;
};

class OrderBook {
public:
    template<typename TradeCB>
    OrderBook(TradeCB&& pred) : cb{std::forward<TradeCB>(pred)} {}

    void update(const srv::NewOrderRequest& order);

    void cancel(const srv::Ids& ids);

    struct PriceQuantity {
        std::uint32_t price;
        std::uint32_t quantity;
    };
    template<bool Side>
    std::optional<std::pair<PriceQuantity, srv::Ids>> getTOB() const {
        const BookSide* recipient = Side ? &buy_side : &sell_side;
        if(recipient->orders.empty()) {
            return std::nullopt;
        }
        auto it = recipient->orders.begin();
        return std::make_pair(PriceQuantity{
            it->first,
            it->second.quantity
        }, it->second.ids);
    }

private:
    std::function<void(srv::Trade&&)> cb;

    void update_();

    struct OrderLocation {
        BookSide::Container* container = nullptr;
        BookSide::Container::iterator iterator;
    };
    // < user id, < order id , info > >
    std::unordered_map<std::string, std::unordered_map<std::string, OrderLocation>> orders_table;
    BookSide buy_side{true};
    BookSide sell_side{false};
};
}
