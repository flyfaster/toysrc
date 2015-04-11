#pragma  once 

struct SafeShardQueue
{
	typedef boost::interprocess::scoped_lock<boost::interprocess::named_mutex> scoped_mutex;
	SafeShardQueue()//:
	{

	}
	~SafeShardQueue()
	{
		remove();
	}
	void wait_msg()
	{
		scoped_mutex locker(*mu);
		data_avail_condition->wait(locker);
	}
	void wait_space()
	{
		scoped_mutex locker(*mu);
		queue_empty_condition->wait(locker);
	}
	boost::shared_ptr<boost::interprocess::message_queue> msgs;
	boost::shared_ptr<boost::interprocess::named_condition> data_avail_condition;
	boost::shared_ptr<boost::interprocess::named_mutex> mu;
	boost::shared_ptr<boost::interprocess::named_condition> queue_empty_condition;

	static SafeShardQueue* open()
	{
		SafeShardQueue* tmp = new SafeShardQueue();
		boost::interprocess::open_only_t open_create_type;
		tmp->msgs.reset(new boost::interprocess::message_queue(open_create_type, "message_queue"));
		tmp->mu.reset(new boost::interprocess::named_mutex(open_create_type, "shared_queue_mutex"));
		tmp->data_avail_condition .reset(new boost::interprocess::named_condition(open_create_type, "data_avail_condition"));
		tmp->queue_empty_condition .reset(new boost::interprocess::named_condition(open_create_type, "queue_empty_condition"));
		return tmp;
	}

	static SafeShardQueue* create()
	{
		remove();
		SafeShardQueue* tmp = new SafeShardQueue();
		boost::interprocess::create_only_t open_create_type;

		std::stringstream ss;
		try
		{
			tmp->msgs.reset(new boost::interprocess::message_queue(open_create_type, "message_queue", 
				QUEUE_SIZE, QUEUE_MSG_SIZE_MAX));
			ss << "message queue created ";
			tmp->mu.reset(new boost::interprocess::named_mutex(open_create_type, "shared_queue_mutex"));
			ss << "named mutex created ";
			tmp->data_avail_condition .reset(new boost::interprocess::named_condition(open_create_type, "data_avail_condition"));
			ss << "named condition (empty) created ";
			tmp->queue_empty_condition .reset(new boost::interprocess::named_condition(open_create_type, "queue_empty_condition"));
			ss << "named condition (full) created ";
		}
		catch (const boost::interprocess::interprocess_exception& e)
		{
			ss << e.what();
			OzIPCDlg::Dump( ss.str() );
		}
		return tmp;
	}
	static void remove()
	{
		boost::interprocess::message_queue::remove("message_queue");
		boost::interprocess::named_mutex::remove("shared_queue_mutex");
		boost::interprocess::named_condition::remove("data_avail_condition");
		boost::interprocess::named_condition::remove("queue_empty_condition");
		OzIPCDlg::Dump(__FUNCTION__);
	}
};
typedef boost::scoped_ptr<SafeShardQueue> scoped_SafeSharedQueue;