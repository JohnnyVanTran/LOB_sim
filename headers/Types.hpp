#pragma once

#include <cstdint>

enum class Side : uint8_t { BUY, SELL };
enum class OrderType : uint8_t { LIMIT, MARKET, STOP };

using OrderId = uint64_t;
using Price = int64_t;
using Quantity = uint64_t;
using Timestamp = uint64_t;

struct Order {
    OrderId id;
    Side side;
    Price price;
    Quantity quantity;
    Timestamp timestamp;

    Order* next{nullptr}; // Pointer to the next order in the linked list
    Order* prev{nullptr}; // Pointer to the previous order in the linked list
};