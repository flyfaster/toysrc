#pragma once 

struct PerformanceProfile
{
	__int64 start;
	__int64 stop;
	__int64 frequency;
	__int64 overhead;
	std::string title;
	PerformanceProfile(const std::string subject) : title(subject)
	{
		QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency);
		QueryPerformanceCounter((LARGE_INTEGER*)&overhead);
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	}
	~PerformanceProfile()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&stop);
		overhead = start-overhead;
		std::stringstream ss;
		ss  << title << " use " << (LPCSTR)share_type << " process " << MAX_MSG_COUNT 
			<< " use " << (double)(stop-start -overhead)/frequency << " seconds, "
			<< "Queue size: " << QUEUE_SIZE
			<< ", MSG_SIZE_MAX: "<< MSG_SIZE_MAX;
		OzIPCDlg::Dump(ss.str());
	}
};