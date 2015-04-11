#pragma  once

class work_thread : public ihttp_event
{
public:
	work_thread()
	{
		std::cout << __FUNCTION__ << std::hex <<std::setfill('0') << std::setw(8) << std::internal << (int)this
			<< std::endl;
		//open_clients();
		active_clients_ = 0;
	}
	~work_thread()
	{
		std::cout << __FUNCTION__ << std::hex <<std::setfill('0') << std::setw(8) << std::internal << (int)this
			<< std::endl;

	}
	void on_complete(ihttp_client* client, boost::system::error_code err)
	{
		//clients_.erase(client->key());
		active_clients_--;
		open_clients();
	}
	void operator()()
	{
		open_clients();
		//while(clients_.size() || stock_symbols.size())
		//	sleep();
		std::cout << __FUNCTION__ << " ended." << std::endl;
	}
private:
	void open_clients()
	{

		while( stock_symbols.size() && active_clients_<MAX_CONCURRENT_CLIENTS)
		{
			std::string stock_symbol = stock_symbols.pop_symbol();
			if (stock_symbol.size()==0)
				break;
			ihttp_client *client=(new http_client(io_service, 
				"finance.google.com",
				std::string("/finance?q=") + stock_symbol,
				stock_symbol + ".txt" ));
			client->add_event_listener(this);
			active_clients_++;
			//clients_[client->key()] = client;
		}

	}
	//std::map<std::string, ptr_ihttp_client > clients_;
	static const int MAX_CONCURRENT_CLIENTS = 2;
	int active_clients_;
};