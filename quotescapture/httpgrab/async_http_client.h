#pragma once 

class async_http_client: public boost::enable_shared_from_this<async_http_client>,
    private boost::noncopyable
{
public:
    ~async_http_client();
    static boost::shared_ptr<async_http_client> create(boost::asio::io_service& io_service,
        const std::string& url, // format: http://blog.think-async.com/2008/05/boostasio-vs-asio.html
        const std::string& path, // valid file path
        unsigned long timeout_ms); // timeout in milliseconds if >0, otherwise it is ignored

//     void enable_timer();
    bool is_valid();
    const boost::system::error_code& get_error() const;
    void do_resolve(  );
private:
    async_http_client(boost::asio::io_service& io_service,
        const std::string& url, // format: http://blog.think-async.com/2008/05/boostasio-vs-asio.html
        const std::string& path, // valid file path
        unsigned long timeout_ms); // timeout in milliseconds if >0, otherwise it is ignored

    void handle_resolve(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);

    void handle_connect(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);

    void handle_write_request(const boost::system::error_code& err);

    void handle_read_status_line(const boost::system::error_code& err);

    void handle_read_headers(const boost::system::error_code& err);

    void handle_read_content(const boost::system::error_code& err);
    void handle_error(const std::string& function_name, const boost::system::error_code& err);
    void timeout_break();
    void handle_timeout(const std::string& function_name);
    void start_timer(const std::string& function_name);
    void close();
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    std::string local_file_path_;
    std::string server_;
    std::string path_;
    unsigned long timeout_ms_;
    std::stringstream ss_;
    boost::asio::deadline_timer timer_;
    boost::posix_time::ptime start_time_;
    boost::posix_time::ptime end_time_;
    boost::system::error_code err_;
};