#pragma once

#include <gtest/gtest.h>

#include <OrderBook.h>

namespace book::testing {
using TOB = std::pair<OrderBook::PriceQuantity, srv::Ids>;
using HistoryStep = std::tuple<std::optional<TOB>, std::optional<TOB>, std::optional<srv::Trade>>;
using History = std::vector<HistoryStep>;

srv::NewOrderRequest make_order(bool side, std::uint32_t price, std::uint32_t qnty, const srv::Ids& ids);

srv::Ids make_ids(const std::string& userId, const std::string& orderId);

History history_from_string(std::string raw);

struct OrderBookFixture : ::testing::Test {
    void setUpExpectedHistory(History&& expected) {
        history_expected = std::forward<History>(expected);
    }

    void newOrder(const srv::NewOrderRequest& order);
    void cancelOrder(const srv::Ids& ids);
    void TearDown() override;

private:
    std::optional<srv::Trade> lastTrade;
    OrderBook book{[this](srv::Trade&& trade){
        lastTrade = std::forward<srv::Trade>(trade);
    }};
    History history;
    History history_expected;
};

}
