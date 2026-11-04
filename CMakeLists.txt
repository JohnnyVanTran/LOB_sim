cmake_minimum_required(VERSION 3.15)
project(LOB_Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lob_sim
    src/main.cpp
    src/LimitOrderBook.cpp
    src/TickStreamer.cpp
)

target_include_directories(lob_sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)

if (MSVC)
    target_compile_options(lob_sim PRIVATE /W4 /wx)
else()
    target_compile_options(lob_sim PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

