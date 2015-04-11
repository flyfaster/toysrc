/*
 * FindThread.cpp
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include "wx/dirdlg.h"
#include "FindThread.h"
#include <iostream>
#include <boost/thread.hpp>
#include "GuiFindDlg.h"
#include "GuiFindApp.h"
void PrintMessage(const char* caption)
{
	time_t tt;
	time(&tt);
	std::cout << caption << " at " << ctime(&tt) << std::endl;
//	wxGetApp().m_find_dlg->AppendResult(caption);
}

void LogMessage(wxString msg)
{
	wxGetApp().m_find_dlg->AppendResult(msg);
}

FindThread::FindThread() {
	m_stop=false;
	wxString msg;
	msg.Printf(_T("FindThread::FindThread(%d)"), (int)this);
	LogMessage(msg);
}

FindThread::~FindThread() {
	LogMessage(_T("FindThread::~FindThread()"));
}

int FindThread::operator()()
{
	Run();
	return 0;
}

void thread_sleep(int secs)
{
boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += secs;
	boost::thread::sleep(xt);
}
void FindThread::Run()
{
	//	wxGetApp().m_find_dlg->AppendResult(_T("Start of FindThread"));
	LogMessage(_T("FindThread::operator()() start"));
	int i=0;
	while(Continue())
	{
		if (i++>5)
			break;
		thread_sleep(1);
//		boost::thread::yield();
		wxString msg;
		msg.Printf(_T("FindThread::operator()() loop in %d i=%d"), this, i);
		LogMessage(msg);
	}
	LogMessage(_T("FindThread::operator()() end"));
	LogMessage(_T("End of FindThread"));
	thread_sleep(2);
}
void FindThread::Stop(bool bval)
{
	m_stop=bval;
	LogMessage(_T("FindThread::Stop()"));
}

bool FindThread::Continue()
{
	return !m_stop;
}

void dummy_thread_func()
{
	wxGetApp().m_find_dlg->AppendResult(_T("dummy_thread_func() start"));
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 1;
	boost::thread::sleep(xt);
	wxGetApp().m_find_dlg->AppendResult(_T("dummy_thread_func() end"));
}
