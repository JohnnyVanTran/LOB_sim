#pragma once

#include <string>
#include "LimitOrderBook.hpp" // Required so the compiler knows what a LimitOrderBook is

void stream_ticks_to_lob(LimitOrderBook& lob, const std::string& filepath);