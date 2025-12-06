//отправляет задачу и ждет на нее ответ

#include <iostream>
#include <vector>
#include <list>
#include <asio.hpp>

class asker
{
    asio::ip::tcp::socket self;
    std::vector<char> message;
    asio::strand<asio::any_io_executor> runner;
public:
    asker(asio::ip::tcp::socket&& self)
    :
        self(std::move(self)),
        message(1024),
        runner(asio::make_strand(self.get_executor()))
    {}

    bool error_hanlder(asio::error_code& ec)
    {
        //...
        return false;
    }

    //обработка ответа
    void answer_handler()
    {
        std::cout.write(message.data(),message.size())<<std::endl;
    }

    void register_task(const std::string text)
    {
        asio::post
        (
            runner,
            [this,text=std::move(text)]()
            {
                message.assign(text.begin(),text.end());
                message.resize(text.size());
                write_register();
            }
        );
    }

    //регистрация отправки задачи
    void write_register()
    {
        asio::async_write
        (
            self,
            asio::buffer(message),
            asio::bind_executor
            (
                runner,
                [this](asio::error_code ec,size_t size)
                {
                    if(error_hanlder(ec))
                        return;
                    
                    read_register();
                }
            )
        );
    }

    //регистрация чтения ответа
    void read_register()
    {
        self.async_read_some
        (
            asio::buffer(message),
            [this](asio::error_code ec,size_t size)
            {
                if(error_hanlder(ec))return;
                message.resize(size);
                answer_handler();
            }
        );
    }
};