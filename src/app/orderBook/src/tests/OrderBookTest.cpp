#include "OrderBookFixture.h"

using OrderBookFixture = book::testing::OrderBookFixture;
using namespace book::testing;

TEST_F(OrderBookFixture, one_order_per_side) {
    std::string expected = R"(
5 10 foo first|-|-
5 10 foo first|7 10 foo second|-
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 5, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 7, 10, make_ids("foo", "second")));
}

TEST_F(OrderBookFixture, one_order_per_side_with_trade) {
    std::string expected = R"(
7 10 foo first|-|-
-|-|foo first 7 T foo second 5 T 10
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 7, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 5, 10, make_ids("foo", "second")));
}

// partial fill

// 2 full fill
