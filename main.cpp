#include <vector>
#include <iostream>
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

    worker a(std::move(server));
    asker b(std::move(client));

    a.read_register();
    b.register_task("abc");

    context.run();

    return 0;
}
