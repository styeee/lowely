cmake_minimum_required(VERSION 3.22)

project(lowley)

find_package(asio REQUIRED)
find_package(GTest REQUIRED)

add_executable(lowley main.cpp)
target_link_libraries(lowley asio::asio ws2_32 mswsock)

add_executable(test_lowley test.cpp)
target_link_libraries(test_lowley asio::asio ws2_32 mswsock)
target_link_libraries(test_lowley GTest::GTest)