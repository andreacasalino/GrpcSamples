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

TEST_F(OrderBookFixture, one_order_per_side_with_exact_fill) {
    std::string expected = R"(
7 10 foo first|-|-
-|-|foo first 7 T foo second 5 T 10
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 7, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 5, 10, make_ids("foo", "second")));
}

TEST_F(OrderBookFixture, one_order_per_side_with_partial_fill) {
    std::string expected = R"(
7 10 foo first|-|-
7 5 foo first|-|foo first 7 F foo second 5 T 5
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 7, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 5, 5, make_ids("foo", "second")));
}

TEST_F(OrderBookFixture, one_order_per_side_with_2_partial_fill) {
    std::string expected = R"(
7 10 foo first|-|-
7 5 foo first|-|foo first 7 F foo second 5 T 5
-|-|foo first 7 T foo third 5 T 5
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 7, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 5, 5, make_ids("foo", "second")));
    newOrder(make_order(false, 5, 5, make_ids("foo", "third")));
}

TEST_F(OrderBookFixture, one_order_then_cancel) {
    std::string expected = R"(
5 10 foo first|-|-
-|-|-
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 5, 10, make_ids("foo", "first")));
    cancelOrder(make_ids("foo", "first"));
}
