#pragma once
#include <iostream>
#include <asio.hpp>

#define PARRALEL_ON

#ifdef PARRALEL_ON
#include <vector>
#include <thread>
#endif

auto address(const char* ip,unsigned short port)
{
    auto ip_addr=asio::ip::address::from_string(ip);
    return asio::ip::tcp::endpoint(ip_addr,port);
}

auto address(std::istream& in)
{
    std::string ip;
    unsigned short port;
    in>>ip>>port;
    return address(ip.c_str(),port);
}

bool error(asio::error_code& ec)
{
    return false;
}

void parrallel_run(asio::io_context& context)
{
    #ifdef PARRALEL_ON
        std::vector<std::thread> threads;
        threads.reserve(std::thread::hardware_concurrency());

        for(size_t i=0;i<threads.capacity();i++)
            threads.emplace_back
            (
                [](asio::io_context& context){context.run();},
                std::ref(context)
            );
        
        for(auto& th:threads)
            th.join();
    #else
        context.run();
    #endif
}