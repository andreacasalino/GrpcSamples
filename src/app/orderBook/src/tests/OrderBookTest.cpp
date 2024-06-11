#include <gtest/gtest.h>

#include <OrderBook.h>

namespace book::testing {
using TOB = std::pair<OrderBook::PriceQuantity, srv::Ids>;
using HistoryStep = std::tuple<std::optional<TOB>, std::optional<TOB>, std::optional<srv::Trade>>;
using History = std::vector<HistoryStep>;

struct LastTradeMemoizer {
    LastTradeMemoizer() = default;

    void operator()(srv::Trade&& trade) const {
        lastTrade = std::forward<srv::Trade>(trade);
    }

    std::optional<srv::Trade> extractLastTrade() {
        return std::move(lastTrade);
    }

private:
    mutable std::optional<srv::Trade> lastTrade;
};

struct OrderBookFixture : ::testing::Test {
    void setUpExpectedHistory(History&& expected) {
        history_expected = std::forward<History>(expected);
    }

    void newOrder(const srv::NewOrderRequest& order) {
        auto& added = history.emplace_back();
        book.update(order);
        std::get<0>(added) = book.getTOB<true>();
        std::get<1>(added) = book.getTOB<false>();
        std::get<2>(added) = tradeMemoizer.extractLastTrade();
    }

    void cancelOrder(const srv::Ids& ids) {
        auto& added = history.emplace_back();
        book.cancel(ids);
        std::get<0>(added) = book.getTOB<true>();
        std::get<1>(added) = book.getTOB<false>();
    }

    void TearDown() override {
        // TODO compare
    }

private:
    LastTradeMemoizer tradeMemoizer;
    OrderBook book{[this](srv::Trade&& trade){
        tradeMemoizer(std::forward<srv::Trade>(trade));
    }};
    History history;
    History history_expected;
};
}

using OrderBookFixture = book::testing::OrderBookFixture;

TEST_F(OrderBookFixture, one_order) {
    // TODO set up expected history

    // TODO push orders cancels
    // TODO push orders cancels
    // TODO push orders cancels
}
