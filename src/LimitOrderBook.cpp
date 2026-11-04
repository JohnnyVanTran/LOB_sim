#include "LimitOrderBook.hpp"
#include <iostream>
#include <iomanip>

void LimitOrderBook::add_limit_order(OrderId id, Side side, Price price, Quantity qty, Timestamp ts) {
    Quantity remaining_qty = qty;

    if (side == Side::BUY) {
        while (remaining_qty > 0 && !asks.empty()) {
            auto best_ask = asks.begin();
            Price best_price = best_ask->first;
            PriceLevel& ask_level = best_ask->second;

            //Incoming price is to low to buy
            if (price < best_price) { break; }
            
            //Handling order
            while (remaining_qty > 0 && ask_level.head != nullptr) {
                Order* resting_order = ask_level.head;

                if (remaining_qty >= resting_order->quantity) {
                    remaining_qty -= resting_order->quantity;

                    std::cout << "[MATCH] Buy " << id << " matched Ask " 
                              << resting_order->id << " | Qty: " << resting_order->quantity 
                              << " @ Price: " << best_price << std::endl;
                    
                    ask_level.remove_order(resting_order);
                    orders.erase(resting_order->id);
                    memory_pool.deallocate(resting_order);
                } else {
                    resting_order->quantity -= remaining_qty;
                    ask_level.total_volume -= remaining_qty;

                    std::cout << "[MATCH] Buy " << id << " partially matched Ask " 
                              << resting_order->id << " | Qty: " << remaining_qty 
                              << " @ Price: " << best_price << std::endl;
                    
                    remaining_qty = 0;
                }
            }

            if (ask_level.head == nullptr) {
                asks.erase(best_ask);
            }
        }
        //Resting phase for buy orders
        if (remaining_qty > 0) {
            //Will get nullpointer if pool is exhausted
            Order* new_order = memory_pool.allocate(id, side, price, qty, ts);
            if (!new_order) {
                std::cerr << "[ERROR] Memory pool exhausted! Cannot add Buy order " << id << std::endl;
                return;
            }
            
            //Can use if-statement here but achieves the same without
            //asks[price].price = price; can be redundant if new order for same price comes
            bids[price].price = price;
            bids[price].append_order(new_order);
            orders[id] = new_order;
            std::cout << "[REST] Added Buy Order " << id << " @ " 
                    << price << " (Qty: " << remaining_qty << ")" << std::endl;
            

        }
    } else { //Repeating the same for sell-orders
        while (remaining_qty > 0 && !bids.empty()) {
            auto best_bid = bids.begin();
            Price best_price = best_bid->first;
            PriceLevel& bid_level = best_bid->second;

            if (price > best_price) { break; } //Sellprice is to high for current bids

            while (remaining_qty > 0 && bid_level.head != nullptr) {

                Order* resting_order = bid_level.head;

                if (remaining_qty >= resting_order->quantity) {
                    remaining_qty -= resting_order->quantity;

                    std::cout << "[MATCH] Sell " << id << " matched Bid " 
                              << resting_order->id << " | Qty: " << resting_order->quantity 
                              << " @ Price: " << best_price << std::endl;
                    
                    bid_level.remove_order(resting_order);
                    orders.erase(resting_order->id);
                    memory_pool.deallocate(resting_order);
                } else {
                    resting_order->quantity -= remaining_qty;
                    bid_level.total_volume -= remaining_qty;

                    std::cout << "[MATCH] Ask " << id << " Partially matched buy " 
                              << resting_order->id << " | Qty: " << remaining_qty
                              << " @ Price: " << best_price << std::endl;
                    
                    remaining_qty = 0;
                }
            }
            if (bid_level.head == nullptr) {
                bids.erase(best_bid);
            }
        }
        if (remaining_qty > 0) {
            Order* new_order = memory_pool.allocate(id, side, price, qty, ts);
            if (!new_order) {
                std::cerr << "[ERROR] Memory pool exhausted! Cannot add Buy order " << id << std::endl;
                return;
            }
            asks[price].price = price;
            asks[price].append_order(new_order);
            orders[id] = new_order;
            
            std::cout << "[REST] Added Sell Order " << id << " @ " 
                    << price << " (Qty: " << remaining_qty << ")" << std::endl;
        }
    }
}

void LimitOrderBook::cancel_order(OrderId id) {
    //Find order in orders-map
    auto it = orders.find(id);
    if (it == orders.end()) {
        std::cout << "[Cancel] Order ID: " << id << " not found " << std::endl;
        return;
    }

    Order* order = it->second;
    Price price = order->price;

    if (order->side == Side::BUY) {
        auto bid_it = bids.find(price);
        if (bid_it != bids.end()) {
            bid_it->second.remove_order(order);
            if (bid_it->second.head == nullptr) {
                bids.erase(bid_it);
            }
        }
    } else {
        auto ask_it = asks.find(price);
        if (ask_it != asks.end()) {
            ask_it->second.remove_order(order);
            if (ask_it->second.head == nullptr) {
                asks.erase(ask_it);
            }
        }
    }
    orders.erase(it);
    memory_pool.deallocate(order);
    //std::cout << "Successfully cancelled order ID: " << id << std::endl;
};

void LimitOrderBook::print_book() const {
    std::cout << "\n -----LOB STATE-----\n" << std::endl;
    std::cout << "Asks (selling orders):" << std::endl;
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        std::cout << " Price: " << std::setw(6) << it->first 
                  << " | Volume: " << std::setw(6) << it->second.total_volume << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
    std::cout << "Bids (buying orders):" << std::endl;
    for (const auto& [price, level] : bids) {
        std::cout << " Price: " << std::setw(6) << price
                  << " | Volume: " << std::setw(6) << level.total_volume << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
};