#pragma once

#include "Types.hpp"
#include "MemoryPool.hpp"
#include <unordered_map>
#include <map>
#include <iostream>

struct PriceLevel {
    Price price {0};
    Quantity total_volume{0};
    Order* head{nullptr};
    Order* tail{nullptr};

    void append_order(Order* order) {
        order->next = nullptr;
        order->prev = tail;
        if (tail) {
            tail->next = order;
        } else {
            head = order;
        }
        tail = order;
        total_volume += order->quantity;
    }
    
    void remove_order(Order* order) {
        //If order doesnt have a prev, its the head
        if (order->prev) {
            order->prev->next = order->next;
        } else {
            head = order->next;
        }
        //If order doesnt have next, its tail
        if (order->next) {
            order->next->prev = order->prev;
        } else {
            tail = order->prev;
        }

        total_volume -= order->quantity;
        order->next = nullptr;
        order->prev = nullptr;
    }
};

class LimitOrderBook{
    private:
        MemoryPool& memory_pool;
        
        //Using map to look up active orders
        std::map<OrderId, Order*> orders;

        //Price levels for bids and asking. 
        //Bids highest price first(decending), asks lowest price first (ascending)
        std::map<Price, PriceLevel, std::greater<Price>> bids;
        std::map<Price, PriceLevel, std::less<Price>> asks;

    public:
        explicit LimitOrderBook(MemoryPool& pool) : memory_pool(pool) {}

        //Core limitbook functions added in the LimitOrderBook.cpp
        void add_limit_order(OrderId id, Side side, Price price, Quantity qty, Timestamp ts);
        void cancel_order(OrderId id);

        //Helper to see tha book
        void print_book() const;
};
