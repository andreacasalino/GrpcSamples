#include "OrderBookFixture.h"

using OrderBookFixture = book::testing::OrderBookFixture;
using namespace book::testing;

TEST_F(OrderBookFixture, one_order) {
    std::string expected = R"(
5 10 foo first|-|-
5 10 foo first|5 10 foo second|-
)";
    setUpExpectedHistory(history_from_string(expected));

    newOrder(make_order(true, 5, 10, make_ids("foo", "first")));
    newOrder(make_order(false, 5, 10, make_ids("foo", "second")));
}
