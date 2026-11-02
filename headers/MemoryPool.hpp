#pragma once

#include "Types.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

class MemoryPool {
    private:
        std::vector<Order> pool; // Vector to hold the allocated memory blocks
        Order* free_List{nullptr}; // Pointer to the head of the free list
    public:
        explicit MemoryPool(size_t max_orders) {
            if (max_orders == 0) {
                throw std::invalid_argument("Memory pool must be greater than 0");
            }

            pool.resize(max_orders);

            for (size_t i = 0; i < max_orders - 1; ++i) {
                pool[i].next = &pool[i + 1];
                pool[i].prev = nullptr;
            }

            pool[max_orders - 1].next = nullptr;
            pool[max_orders - 1].prev = nullptr;

            free_List = &pool[0]; // Initialize the free list to point to the first order
        }

        // Banning copy constructor and assignment operator to prevent copying of the memory pool
        MemoryPool(const MemoryPool&) = delete; 
        MemoryPool& operator=(const MemoryPool&) = delete; 

        //Allow moving semantics
        MemoryPool(MemoryPool&&) noexcept = default;
        MemoryPool& operator=(MemoryPool&&) noexcept = default;

        ~MemoryPool() = default;

        //O(1) allocation
        Order* allocate(OrderId id, Side side, Price price, Quantity qty, Timestamp ts) {
            if (!free_List) return nullptr;

            Order* order = free_List;
            free_List = free_List->next;    //Moving free_list pointer

            order->id = id;
            order->side = side;
            order->price = price;
            order->quantity = qty;
            order->timestamp = ts;
            order->next = nullptr;
            order->prev = nullptr;

            return order;
        }

        void deallocate(Order* order) {
            if (!order) return;

            order->next = free_List;
            order->prev = nullptr;
            free_List = order;
        }
};