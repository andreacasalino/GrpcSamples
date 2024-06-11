#include <OrderBook.h>

namespace book {
void OrderBook::update(const srv::NewOrderRequest& order_req) {
    const auto& order = order_req.order();
    BookSide* recipient = order.side() ? &buy_side : &sell_side;
    BookSide::Info info;
    info.ids.CopyFrom(order_req.ids());
    info.quantity = order.quantity();
    auto it = recipient->orders.emplace(order.price(), std::move(info));
    orders_table[order_req.ids().userid()][order_req.ids().orderid()] = OrderLocation{
        &recipient->orders,
        it
    };
    update_();
}    

void OrderBook::cancel(const srv::Ids& ids) {
    auto& slot = orders_table.at(ids.userid());
    auto slot_it = slot.find(ids.orderid());
    auto [container, it] = slot_it->second;
    container->erase(it);
    slot.erase(slot_it);
}

void OrderBook::update_() {
    while (!buy_side.orders.empty() && !sell_side.orders.empty()) {
        auto top_buy_it = buy_side.orders.begin();
        auto top_sell_it = sell_side.orders.begin();
        if(top_sell_it->first > top_buy_it->first) {
            break;
        }
        std::uint32_t quantity = std::min<std::uint32_t>(top_sell_it->second.quantity, top_buy_it->second.quantity);
        srv::Trade trade;
        trade.mutable_buyids()->CopyFrom(top_buy_it->second.ids);
        trade.set_buyprice(top_buy_it->first);
        trade.mutable_sellids()->CopyFrom(top_sell_it->second.ids);
        trade.set_sellprice(top_sell_it->first);
        trade.set_quantity(quantity);
        if(quantity == top_buy_it->second.quantity) {
            const auto& ids = top_buy_it->second.ids;
            orders_table[ids.userid()].erase(ids.orderid());
            buy_side.orders.erase(top_buy_it);
            trade.set_buyconsumed(true);
        }
        else {
            top_buy_it->second.quantity -= quantity;
            trade.set_buyconsumed(false);
        }
        if(quantity == top_sell_it->second.quantity) {
            const auto& ids = top_sell_it->second.ids;
            orders_table[ids.userid()].erase(ids.orderid());
            sell_side.orders.erase(top_sell_it);
            trade.set_sellconsumed(true);
        }
        else {
            top_sell_it->second.quantity -= quantity;
            trade.set_sellconsumed(false);
        }
        cb(std::move(trade));
    }
}
}
