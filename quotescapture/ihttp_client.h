#pragma once

class ihttp_event;
typedef boost::shared_ptr<ihttp_event> ihttp_event_ptr;
typedef ihttp_event* ihttp_event_p;

class STOCKCLIENT_API ihttp_client 
    : public boost::enable_shared_from_this<ihttp_client>
{
public:
	virtual void close()=0;
	virtual std::string key()=0;
	virtual void add_event_listener(ihttp_event_ptr e)=0;
	virtual int elapsed_ms() = 0;
	virtual std::string get_url() = 0;
	virtual std::string get_filename() = 0;
    virtual void set_filename(const std::string& filename)=0;
	virtual size_t get_total_read_bytes() = 0;
};
typedef boost::shared_ptr<ihttp_client> ihttp_client_ptr;
//typedef ihttp_client* ihttp_client_p;
typedef ihttp_client_ptr ihttp_client_p;
class ihttp_event
    : public boost::enable_shared_from_this<ihttp_event>
{
public:
	virtual void on_complete(ihttp_client_p client, 
        boost::system::error_code err)=0;
};
