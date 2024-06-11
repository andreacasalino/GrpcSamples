#include "OrderBookFixture.h"
#include <Error.h>

#include <string_view>

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

namespace {
template<char Sep, typename StringT>
std::vector<std::string_view> split(const StringT& line) {
    std::vector<std::string_view> res;
    std::size_t pos = 0;
    while (pos < line.size()) {
        auto next = line.find(Sep, pos);
        if(next == std::string::npos) {
            res.emplace_back(line.data() + pos);
            break;
        }
        res.emplace_back(line.data() + pos, next - pos);
        pos = next + 1;
    }
    return res;
}

std::uint32_t my_atoi(const std::string_view& subject) {
    std::string buffer{subject.data(), subject.size()};
    return static_cast<std::uint32_t>(std::atoi(buffer.c_str()));
}

void parse_ids(srv::Ids& recipient, std::vector<std::string_view>::const_iterator giver) {
    auto convert = [&giver]() {
        const auto& buffer = *giver;
        return std::string{buffer.data(), buffer.size()};
        ++giver;
    };
    recipient.set_userid(convert());
    recipient.set_orderid(convert());
}

std::optional<TOB> parse_tob(const std::string_view& subject) {
    if(subject == "-") {
        return std::nullopt;
    }
    std::optional<TOB> res;
    auto slices = split<' '>(subject);
    auto& val = res.emplace();
    val.first.price = my_atoi(slices[0]);
    val.first.quantity = my_atoi(slices[1]);
    parse_ids(val.second, slices.begin() + 2);
    return res;
}

std::optional<srv::Trade> parse_trade(const std::string_view& subject) {
    if(subject == "-") {
        return std::nullopt;
    }
    std::optional<srv::Trade> res;
    auto slices = split<' '>(subject);
    auto& val = res.emplace();
    parse_ids(*val.mutable_buyids(), slices.begin());
    val.set_buyprice(my_atoi(slices[2]));
    val.set_buyconsumed(slices[3] == "T");
    parse_ids(*val.mutable_sellids(), slices.begin() + 3);
    val.set_sellprice(my_atoi(slices[4]));
    val.set_sellconsumed(slices[5] == "T");
    val.set_quantity(my_atoi(slices[6]));
    return res;
}
}

History history_from_string(std::string raw) {
    History res;
    std::istringstream stream{raw};
    while (!stream.eof()) {
        std::string line;
        getline(stream, line);
        if(line.back() == '\n') {
            line.pop_back();
        }
        if(line.empty()) {
            continue;
        }
        auto slices = split<'|'>(line);
        if(slices.size() != 3) {
            THROW_ERROR("Invalid");
        }
        auto& added = res.emplace_back();
        std::get<0>(added) = parse_tob(slices[0]);
        std::get<1>(added) = parse_tob(slices[1]);
        std::get<2>(added) = parse_trade(slices[2]);
    }
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

namespace {
bool operator==(const srv::Ids& a, const srv::Ids& b) {
    return a.userid() == b.userid() && 
           a.orderid() == b.orderid();
}

bool operator==(const TOB& a, const TOB& b) {
    return a.first.price == b.first.price &&
           a.first.quantity == b.first.quantity &&
           a.second == b.second;
}

bool operator==(const srv::Trade& a, const srv::Trade& b) {
    return a.buyids() == b.buyids() &&
           a.buyprice() == b.buyprice() &&
           a.buyconsumed() == b.buyconsumed() &&
           a.sellids() == b.sellids() &&
           a.sellprice() == b.sellprice() &&
           a.sellconsumed() == b.sellconsumed() &&
           a.quantity() == b.quantity();
}
}

void OrderBookFixture::TearDown() {
    EXPECT_EQ(history, history_expected) << "Output produced by the book is not the expected one";
}
}
