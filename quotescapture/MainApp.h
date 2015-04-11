#pragma once

struct PrivateMainApp;
class http_client_factory;

class STOCKCLIENT_API MainApp :
	public wxApp
{
public:
	MainApp(void);
	~MainApp(void);
	bool OnInit(void);
	void StopTcpService();
	int OnExit();
	int FilterEvent(wxEvent& event);
	static bool Stopping();
	template<class MutexType>
	static void Idle(int idle_milliseconds, MutexType* lockobj=NULL, 
		boost::condition_variable_any* cond=NULL)
	{
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		//xt.nsec += idle_milliseconds * 1000000;
		//xt.sec += idle_milliseconds;
		xt.nsec += idle_milliseconds*1000;
		if (lockobj==NULL || cond==NULL)
		{
			boost::thread::sleep(xt); // Sleep for 1 second
		}
		else
		{
			boost::unique_lock<MutexType> lock(*lockobj);
			cond->timed_wait(lock, xt);
		}
	}
	void ParseCommandLine();
	void StartTcpService();
	void Dump();
	static void RunApp();

	static boost::posix_time::ptime LocalToNY(const boost::posix_time::ptime& shanghai_ptime);
	static boost::posix_time::ptime NYToLocal(const boost::posix_time::ptime& ny_ptime);

	//************************************
	// Method:    LocalToNYMarketDate
	// Brief:     Convert local time to market time (working days)
	// Returns:   boost::posix_time::ptime
	// Parameter: const boost::posix_time::ptime & src
	//************************************
	static boost::posix_time::ptime LocalToNYMarketDate(const boost::posix_time::ptime& shanghai_ptime);
	static boost::posix_time::ptime NYToMarketDate(const boost::posix_time::ptime& ny_ptime);
	static LPCTSTR GetAppPath();
	http_client_factory* GetHttpClientFactory();
	PrivateMainApp* m_data;
private:

};
 std::string STOCKCLIENT_API wx2std(wxString s);
DECLARE_APP(MainApp);


