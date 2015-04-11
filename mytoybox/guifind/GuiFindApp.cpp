/*
 * GuiFindApp.cpp
 *
 *  Created on: 7 Apr 2010
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
#include <boost/thread.hpp>
#include "FindThread.h"
#include "GuiFindApp.h"
#include "GuiFindDlg.h"
#include "VerboseDialog.h"

IMPLEMENT_APP(GuiFindApp)

GuiFindApp::GuiFindApp() {
	m_find_handler=NULL;

}

GuiFindApp::~GuiFindApp() {
	if (m_find_handler)
	{
		m_find_handler->Stop(true);
	}
}
int GuiFindApp::OnExit()
{
//	wxLogMessage(_T("GuiFindApp::OnExit()"));
	wxLog::FlushActive();
	return 0;
}
bool GuiFindApp::OnInit()
{
	//GuiFindDlg dlg(NULL);
	//    dlg.ShowModal();
	m_log_wnd = NULL;
	m_find_dlg=new GuiFindDlg(NULL);
	m_find_dlg->Center();
	m_find_dlg->Show(true);
//	m_log_wnd=new VerboseDialog(m_find_dlg);
//	m_log_wnd->Show(true);
	SetTopWindow(m_find_dlg);
	new boost::thread(dummy_thread_func);
	return true;
}

void GuiFindApp::StartFind(std_string filenamepatter,
		std_string contentpattern, std_string rootpath)
{
	if (m_find_handler==NULL)
		m_find_handler=new FindThread;
	else
		m_find_handler->Stop(true);
	m_find_handler->m_content_pattern=contentpattern;
	m_find_handler->m_file_name_pattern=filenamepatter;
	m_find_handler->m_root_path=rootpath;
	m_find_handler->Stop(false);
	boost::thread *pbt = NULL;
//	pbt = new boost::thread(boost::ref(*m_find_handler));
	pbt = new boost::thread(dummy_thread_func);
//	new boost::thread(dummy_thread_func);
//	pbt->detach();
//	m_find_thread=boost::shared_ptr<boost::thread>(pbt);
//	boost::thread(dummy_thread_func);
}
