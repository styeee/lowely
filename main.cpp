#include <vector>
#include <iostream>
#include <thread>
#include "net.h"
#include <asio.hpp>
#include "asker.h"
#include "worker.h"

int main()
{
    std::cout<<"Start:"<<std::endl;

    auto addr=address("127.0.0.1",11111);

    asio::io_context context;
    asio::ip::tcp::acceptor listener(context);
    listener.open(asio::ip::tcp::v4());
    listener.bind(addr);
    listener.listen();
    asio::ip::tcp::socket server(context);
    asio::ip::tcp::socket client(context);

    client.connect(addr);
    listener.accept(server);

    worker a
    (
        std::move(server),
        [](std::vector<char>& message)
        {
            if(message[0]=='!'&&message[1]=='q')
                return false;
            
            std::reverse(message.begin(),message.end());
            return true;
        }
    );

    asker b(std::move(client));

    std::thread([](asker& b)
    {
        while(true)
        {
            std::string text;
            std::getline(std::cin,text);
            if(text=="quit")exit(0);
            b.register_task
            (
                text,
                [](std::vector<char>& message)
                {
                    std::cout.write(message.data(),message.size());
                    std::cout<<std::endl;
                }
            );
        }
    },
    std::ref(b)).detach();

    context.run();

    return 0;
}
