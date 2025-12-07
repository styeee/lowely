#include <string>
#include <asio.hpp>
#include "worker.h"
#include "asker.h"
#include "net.h"
#include <gtest/gtest.h>

TEST(lowley,simple_reverse)
{
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
    
    b.register_task
    (
        "hello",
        [&b](std::vector<char>& message)
        {
            std::string temp(message.begin(),message.end());
            ASSERT_EQ(temp,"olleh");
            b.register_task("!q",0);
        }
    );

    context.run();
}

int main(int argc,char** argv)
{
	testing::InitGoogleTest(&argc,argv);
	return RUN_ALL_TESTS();
}
