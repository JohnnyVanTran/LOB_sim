#include "LimitOrderBook.hpp"
#include <iostream>
#include <iomanip>

void LimitOrderBook::add_limit_order(OrderId id, Side side, Price price, Quantity qty, Timestamp ts) {
    Quantity remaining_qty = qty;

    if (side == Side::BUY) {
        while (remaining_qty > 0 && !asks.empty()) {
            
        }
    }
}