cmake_minimum_required(VERSION 3.22)

project(lowley)

find_package(LuaJIT REQUIRED)
find_package(asio REQUIRED)

add_executable(lowley main.cpp)
target_link_libraries(lowley asio::asio ws2_32 mswsock)
target_link_libraries(lowley LuaJIT::LuaJIT)