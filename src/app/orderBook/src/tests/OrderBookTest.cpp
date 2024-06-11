#include <gtest/gtest.h>

#include <OrderBook.h>

namespace book::testing {
using TOB = std::pair<OrderBook::PriceQuantity, srv::Ids>;
using HistoryStep = std::tuple<std::optional<TOB>, std::optional<TOB>, std::optional<srv::Trade>>;
using History = std::vector<HistoryStep>;

struct OrderBookHistory {
    void operator()(srv::Trade&& trade) const;

    History history;
};

struct OrderBookFixture : ::testing::Test {
    void setUpExpectedHistory(std::vector<HistoryStep>&& expected);

    void newOrder(const srv::NewOrderRequest& order);

    void cancelOrder(const srv::Ids& ids);

    void TearDown() override;

private:
    template<typename Pred>
    void useBook_(Pred pred);

    OrderBookHistory history;
    OrderBook book{[this](srv::Trade&& trade){
        history(std::forward<srv::Trade>(trade));
    }};
};
}

TEST_F(book::testing::OrderBookFixture, one_order) {
    // TODO set up expected history

    // TODO push orders cancels
    // TODO push orders cancels
    // TODO push orders cancels
}
