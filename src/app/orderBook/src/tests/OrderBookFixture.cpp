#include "OrderBookFixture.h"

namespace book::testing {
srv::NewOrderRequest make_order(bool side, std::uint32_t price, std::uint32_t qnty, const srv::Ids& ids) {
    srv::NewOrderRequest res;
    res.mutable_order()->set_side(side);
    res.mutable_order()->set_price(price);
    res.mutable_order()->set_quantity(qnty);
    res.mutable_ids()->CopyFrom(ids);
    return res;
}

srv::Ids make_ids(const std::string& userId, const std::string& orderId) {
    srv::Ids res;
    res.set_userid(userId);
    res.set_orderid(orderId);
    return res;
}

void OrderBookFixture::newOrder(const srv::NewOrderRequest& order) {
    auto& added = history.emplace_back();
    book.update(order);
    std::get<0>(added) = book.getTOB<true>();
    std::get<1>(added) = book.getTOB<false>();
    std::get<2>(added) = std::move(lastTrade);
    lastTrade.reset();
}

void OrderBookFixture::cancelOrder(const srv::Ids& ids) {
    auto& added = history.emplace_back();
    book.cancel(ids);
    std::get<0>(added) = book.getTOB<true>();
    std::get<1>(added) = book.getTOB<false>();
}

void OrderBookFixture::TearDown() {
    // TODO compare
}
}
