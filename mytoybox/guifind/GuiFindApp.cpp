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
#include <boost/program_options.hpp>
#include <unordered_set>
#include <deque>
#include <vector>
#include <boost/regex.hpp>
#include "FindThread.h"
#include "GuiFindApp.h"
#include "GuiFindDlg.h"
#include "VerboseDialog.h"

IMPLEMENT_APP(GuiFindApp)

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
	boost::program_options::variables_map vm;
    boost::program_options::options_description desc("Allowed options");
	desc.add_options()("help,h", "produce help message")
	  ;
	boost::program_options::store(
			boost::program_options::parse_command_line(wxGetApp().argc, (char**)wxGetApp().argv, desc), vm);
	boost::program_options::notify(vm);
	if(vm.count("help")) {
		std::cout << desc << std::endl;
		std::cout << "File Name: specify regex expression of file name that is interested\n";
		std::cout << "Exclude File Name: specify regex expression of file name that should be excluded (not display in result area)\n";
		return 0;
	}
	//GuiFindDlg dlg(NULL);
	//    dlg.ShowModal();
	m_find_dlg=new GuiFindDlg();
	m_find_dlg->Center();
	m_find_dlg->Show(true);
//	m_log_wnd=new VerboseDialog(m_find_dlg);
//	m_log_wnd->Show(true);
	SetTopWindow(m_find_dlg);
//	new boost::thread(dummy_thread_func);
	return true;
}

void GuiFindApp::StartFind()
{
	std_string filenamepatter = properties[wxString("m_filename_pattern_tc").ToStdWstring()];
	std_string contentpattern = properties[wxString("m_content_pattern_tc").ToStdWstring()];
	std_string rootpath = properties[wxString("m_root_path_tc").ToStdWstring()];
	std_string exclude_filenames = properties[wxString("m_filename_pattern_exclude").ToStdWstring()];

	if (m_find_handler==NULL)
		m_find_handler=new FindThread;
	else
		m_find_handler->Stop(true);
	m_find_handler->m_content_pattern=contentpattern;
	m_find_handler->m_file_name_pattern=filenamepatter;
	m_find_handler->m_file_name_pattern_exclude = exclude_filenames;
	m_find_handler->m_root_path=rootpath;
	m_find_handler->Stop(false);
	boost::thread *pbt = NULL;
	pbt = new boost::thread(boost::ref(*m_find_handler));
//	pbt = new boost::thread(dummy_thread_func);
//	new boost::thread(dummy_thread_func);
//	pbt->detach();
//	m_find_thread=boost::shared_ptr<boost::thread>(pbt);
//	boost::thread(dummy_thread_func);
}

void GuiFindApp::SetProperty(std_string key, std_string value) {
	properties[key] = value;
}
