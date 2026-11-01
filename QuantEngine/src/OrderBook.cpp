#include "./OrderBook.hpp"
#include <iostream>
#include <algorithm>

Price OrderBook::get_best_bid() const {
    if (bids.empty()) return 0;
    return bids.begin()->first;
}

Price OrderBook::get_best_ask() const {
    if (asks.empty()) return 0;
    return asks.begin()->first;
}

void OrderBook::insert_limit_order(Order order) {
    //Step 1: match incoming order with resting orders on oppisite side
    if (order.side == Side::Buy) {
        while (order.qty > 0 && !asks.empty() && order.price >= asks.begin()->first) {
            auto& best_ask_level = asks.begin()->second;
            auto& orders_list = best_ask_level.orders;

            while (order.qty > 0 && !orders_list.empty()) {
                auto& resting_order = orders_list.front();

                //Deterimine the trade execution size
                uint32_t match_qty = std::min(order.qty, resting_order.qty);

                //Execute trades and reduce volume
                order.qty -= match_qty;
                resting_order.qty -= match_qty;
                best_ask_level.total_volume -= match_qty;

                std::cout <<"TRADE: Order " << order.id << " matched with order " <<resting_order.id 
                << " Qty: " << match_qty << " @ Price: " << resting_order.price << "\n";

                if (resting_order.qty == 0) {
                    order_map.erase(resting_order.id);
                    orders_list.pop_front();
                }
                
            }
            
        }
        
    }
}