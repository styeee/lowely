//принимает задачу и отвечает решением

#include <vector>
#include <list>
#include <asio.hpp>

class worker
{
    asio::ip::tcp::socket self;
    std::vector<char> message;
    asio::strand<asio::any_io_executor> runner;

    typedef std::function<bool(std::vector<char>&)>
        read_hanlder_type;
    read_hanlder_type read_hanlder;
public:
    void stop_register()
    {
        asio::post
        (runner,
            [this]()
            {
                self.cancel();
                self.close();
            }
        );
    }

    worker
    (
        asio::ip::tcp::socket&& self,
        read_hanlder_type read_hanlder
    )
    :
        read_hanlder(read_hanlder),
        message(1024),
        self(std::move(self)),
        runner(asio::make_strand(this->self.get_executor()))
    {read_register();}
    ~worker()
    {stop_register();}

    bool error_hanlder(asio::error_code& ec)
    {
        //...
        return false;
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
                if(read_hanlder(message))
                    answer_register();
                else
                    stop_register();
            }
        );
    }

    //регистрация поведения после отправки ответа
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

                    message.resize(message.capacity());
                    read_register();
                }
            )
        );
    }
};