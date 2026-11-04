#include <iostream>
#include <fstream>
#include <charconv>
#include <string>
#include <string_view>
#include <chrono>
#include "LimitOrderBook.hpp" // Your matching engine header

void stream_ticks_to_lob(LimitOrderBook& lob, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open tick file: " << filepath << std::endl;
        return;
    }

    std::string line;
    // 1. Skip the CSV header row (timestamp,side,price,quantity)
    if (!std::getline(file, line)) {
        std::cerr << "[ERROR] The tick file is empty." << std::endl;
        return;
    }

    long long tick_count = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Stream line by line to prevent massive memory spikes
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        //comma-index splitting (avoids heavy std::stringstream allocations)
        size_t comma1 = line.find(',');
        size_t comma2 = line.find(',', comma1 + 1);
        size_t comma3 = line.find(',', comma2 + 1);

        if (comma1 == std::string::npos || comma2 == std::string::npos || comma3 == std::string::npos) {
            continue; // Skip malformed lines gracefully
        }

        // Use string_view for zero-copy views into the line buffer
        std::string_view line_view(line);
        std::string_view ts_sv    = line_view.substr(0, comma1);
        std::string_view side_sv  = line_view.substr(comma1 + 1, comma2 - (comma1 + 1));
        std::string_view price_sv = line_view.substr(comma2 + 1, comma3 - (comma2 + 1));
        std::string_view qty_sv   = line_view.substr(comma3 + 1);

        // Parse numerical values safely
        uint64_t timestamp = 0;
        double raw_price = 0.0;
        double raw_qty = 0.0;
        
        //Incompatible with clang
        /* std::from_chars(price_sv.data(), price_sv.data() + price_sv.size(), raw_price);
        std::from_chars(qty_sv.data(), qty_sv.data() + qty_sv.size(), raw_qty);
 */
        try { // Convert string_view slices to temporary std::string for safe parsing
            timestamp = std::stoul(std::string(ts_sv));
            raw_price = std::stod(std::string(price_sv));
            raw_qty = std::stod(std::string(qty_sv));
        } catch (const std::exception& e) {
            continue; // Skip line if price or quantity is malformed
        }
        // Map side string to your engine's Side enum
        Side side = (side_sv == "BUY") ? Side::BUY : Side::SELL;

        // Apply fixed-point scaling (e.g., multiply by 100.0 or 10000.0 to avoid float rounding errors)
        Price price = static_cast<Price>(raw_price * 100.0);
        Quantity qty = static_cast<Quantity>(raw_qty * 10000.0);

        // 3. Fire the event into your matching engine
        // Using tick_count as a deterministic mock OrderId for historical simulation
        lob.add_limit_order(static_cast<OrderId>(tick_count), side, price, qty, timestamp);

        tick_count++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    double tps = (duration_ms > 0) ? (static_cast<double>(tick_count) / duration_ms) * 1000.0 : static_cast<double>(tick_count);

    std::cout << "[BACKTEST COMPLETE]\n"
              << "----------------------------------------\n"
              << " Total Ticks Processed: " << tick_count << "\n"
              << " Execution Time:        " << duration_ms << " ms\n"
              << " Throughput:            " << static_cast<long long>(tps) << " ticks/sec\n"
              << "----------------------------------------" << std::endl;
}