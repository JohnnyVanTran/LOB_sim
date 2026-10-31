#pragma once
#include <cstdint>

enum class Side {Buy,Sell};

using Price = int64_t;
using OrderID = uint64_t;

struct Order {
    OrderID id;
    Price price;
    uint32_t qty;
    Side side;
    uint64_t timestamp;
};