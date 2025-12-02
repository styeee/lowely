cmake_minimum_required(VERSION 3.22)

project(lowley)

find_package(LuaJIT REQUIRED)
find_package(asio REQUIRED)

add_executable(client client.cpp)
target_link_libraries(client LuaJIT::LuaJIT)
target_link_libraries(client asio::asio)

add_executable(server server.cpp)
target_link_libraries(server asio::asio ws2_32 mswsock)