#pragma once
#ifndef _TIME_WATCHER_HPP_ONEGA_
#define _TIME_WATCHER_HPP_ONEGA_
#include "onegadebug.h"
class TimeWatcher
{
public:
	TimeWatcher(std::string descrp):description(descrp)
	{
		BOOL  bret=QueryPerformanceFrequency((LARGE_INTEGER*)&lfreq);
		QueryPerformanceCounter((LARGE_INTEGER*)&lstart);
	}
	~TimeWatcher()
	{
		__int64  lstop;
		QueryPerformanceCounter((LARGE_INTEGER*)&lstop);
		__int64 lruntime=lstop - lstart;
		double dsec= (double)lruntime/lfreq;
		//CString text;
		//text.Format("%s use time %f",description.c_str(),dsec);
		//OnegaDebug::Dump(text);
		OnegaDebug::Dump(_T("%s use time %f"),description.c_str(),dsec);
	}
	__int64  lfreq;
	__int64  lstart;
	std::string description;
};
#endif