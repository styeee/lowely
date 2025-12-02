#include <iostream>
#include <vector>
#include <functional>
#include <asio.hpp>
#include "net.h"

class session
{
    typedef size_t id_type;
    
    static std::vector<session*> all;
    asio::ip::tcp::socket self;
    asio::strand<asio::any_io_executor> runner;

    id_type id;
    id_type to;

    std::vector<char> message;

    void read()
    {
        self.async_read_some
        (
            asio::buffer(message),
            [this](asio::error_code ec,size_t size)
            {
                if(error(ec))
                    {stop(id);return;}
                    
                message.resize(size);
                std::reverse(message.begin(),message.end());
                write();
            }
        );
    }

    void write()
    {
        if(!all[to]){read();return;}

        asio::async_write
        (
            all[to]->self,
            asio::buffer(message),
            asio::bind_executor
            (
                runner,
                [this](asio::error_code ec,size_t size)
                {
                    if(error(ec))
                        {stop(id);return;}
                    
                    message.resize(message.capacity());
                    read();
                }
            )
        );
    }

    session(asio::ip::tcp::socket&& self,size_t id)
    :
        self(std::move(self)),
        message(1024),
        id(id),
        to(id),
        runner(asio::make_strand(this->self.get_executor()))
    {}
    ~session()
    {
        self.cancel();
        self.close();
    }
public:
    static id_type create(asio::ip::tcp::socket&& self)
    {
        const id_type size=all.size();
        all.push_back(new session(std::move(self),size));
        all.back()->read();
        return size;
    }

    static void stop(id_type id)
    {
        delete all[id];
        all[id]=nullptr;
    }
};
std::vector<session*> session::all;

int main()
{
    std::cout<<"Server:\n";

    asio::io_context c;
    asio::ip::tcp::acceptor a(c);
    a.open(asio::ip::tcp::v4());
    std::cout<<"Enter ip and port of server via space:";
    a.bind(address(std::cin));
    a.listen();
    
    std::function<void(asio::ip::tcp::socket)> 
        accept=[&](asio::ip::tcp::socket t)
    {
        a.async_accept
        (
            [&]
            (
                asio::error_code ec,
                asio::ip::tcp::socket&& t
            )
            {
                if(!error(ec))
                    accept(std::move(t));
            }
        );

        if(t.is_open())
            session::create(std::move(t));
    };
    accept(std::move(asio::ip::tcp::socket(c)));

    c.run();

    return 0;
}
