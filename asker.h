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

    typedef std::function<void(std::vector<char>&)>
        read_hanlder_type;
    read_hanlder_type answer_handler;
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

    void register_task
    (
        const std::string text,
        read_hanlder_type answer_handler
    )
    {
        this->answer_handler=std::move(answer_handler);

        asio::post
        (
            runner,
            [this,text]()
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
                    
                    if(answer_handler)
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
                answer_handler(message);
            }
        );
    }
};