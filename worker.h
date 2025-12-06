//принимает задачу и отвечает решением

#include <vector>
#include <list>
#include <asio.hpp>

class worker
{
    asio::ip::tcp::socket self;
    std::vector<char> message;
    asio::strand<asio::any_io_executor> runner;
public:
    worker(asio::ip::tcp::socket&& self)
    :
        message(1024),
        self(std::move(self)),
        runner(asio::make_strand(this->self.get_executor()))
    {read_register();}
    ~worker()
    {
        self.cancel();
        self.close();
    }

    bool error_hanlder(asio::error_code& ec)
    {
        //...
        return false;
    }

    //решение задачи
    void read_hanlder()
    {
        //...
        std::reverse(message.begin(),message.end());
    }

    //регистрация поведения при принятии задачи
    void read_register()
    {
        self.async_read_some
        (
            asio::buffer(message),
            [this](asio::error_code ec,size_t size)
            {
                if(error_hanlder(ec))return;
                message.resize(size);
                read_hanlder();
                answer_register();
            }
        );
    }

    //регистрация поведения при отправке ответа
    void answer_register()
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
};