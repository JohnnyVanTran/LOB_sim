#pragma once
#include "Order.hpp"
#include <map>
#include <unordered_map>
#include <list>

struct LimitLevel {
    uint32_t total_volume = 0;
    std::list<Order> orders; //Time priority FIFO
};

class OrderBook {
    private:
        // Bids sorted descending (highest bought first)
        std::map<Price, LimitLevel, std::greater<Price>> bids;

        //Asks sorted ascending (lowest sell price first)
        std::map<Price, LimitLevel, std::less<Price>> asks;

        //Fast lookup map pointing directly to list nodes for O(1) cancellations
        std::unordered_map<OrderID, std::pair<Side, std::list<Order>::iterator>> order_map;

    public:
        OrderBook() = default;
        ~OrderBook() = default;

        //Matching Engine Functions
        void insert_limit_order(Order order);
        void cancel_order(OrderID id);

        //Getters for the top of the book prices
        Price get_best_bid() const;
        Price get_best_ask() const;
};