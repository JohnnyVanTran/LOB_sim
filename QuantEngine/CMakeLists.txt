cmake_minimum_required(VERSION 3.15)
project(QuantEngine VERSION 1.0 LANGUAGES CXX)

#Require C++20 for modern features and performance optimization
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(QuantEngine
    src/main.cpp
    src/OrderBook.cpp
)