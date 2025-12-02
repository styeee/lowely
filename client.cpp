#include <iostream>
#include <string>
#include <asio.hpp>
#include "net.h"

int main()
{
    std::cout<<"Cleint:\n";

    asio::io_context c;
    asio::ip::tcp::socket s(c);
    s.open(asio::ip::tcp::v4());

    std::cout<<"Enter ip and port of server via space:";
    s.connect(address(std::cin));
    std::cout<<s.local_endpoint()<<std::endl;

    std::string text;

    while(true)
    {
        std::getline(std::cin,text);
        s.send(asio::buffer(text.data(),text.size()));
        asio::read(s,asio::buffer(text.data(),text.size()));
        std::cout<<text<<std::endl;
    }

    return 0;
}