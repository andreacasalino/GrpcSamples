#include <gtest/gtest.h>

#include <OrderBook.h>

using namespace book;

struct OrderBookHistory {
    void operator()(srv::Trade&& trade) const;
};

struct OrderBookFixture : ::testing::Test {
    void SetUp() override {}


    OrderBookHistory history;
    OrderBook book{[this](srv::Trade&& trade){
        history(std::forward<srv::Trade>(trade));
    }};
};

TEST_F(OrderBookFixture, one_order) {

}
