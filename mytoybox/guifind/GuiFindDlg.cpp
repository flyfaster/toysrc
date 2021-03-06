/*
 * GuiFindDlg.cpp
 *
 *  Created on: 8 Apr 2010
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/utils.h>
#include <wx/stdpaths.h>
#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include <wx/timer.h>
#include "wx/dirdlg.h"
#include <boost/thread/mutex.hpp>
#include <unordered_set>
#include <deque>
#include <vector>
#include <boost/regex.hpp>
#include "GuiFindDlg.h"
#include "GuiFindApp.h"
#include "FindThread.h"
int WORKER_EVENT = 111222;

IMPLEMENT_DYNAMIC_CLASS( GuiFindDlg, wxFrame )

BEGIN_EVENT_TABLE(GuiFindDlg, wxFrame)
//	EVT_BUTTON(wxID_ANY, GuiFindDlg::OnButton)
	EVT_COMMAND(WORKER_EVENT, wxEVT_NULL, GuiFindDlg::OnUpdateResult)
//	EVT_CUSTOM(wxCUSTOM_EVENT, wxID_ANY, GuiFindDlg::OnCustomEvent)
	EVT_CUSTOM_EVENT( GuiFindDlg::OnAppendResult )
END_EVENT_TABLE()
void GuiFindDlg::OnCustomEvent(wxEvent &evt)
{
	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__ << std::endl;
	m_log.flush();
}
int TIMER_ID = 100;
int btn_id_base = 7000;
int btn_cancel_id = btn_id_base+__LINE__;
int btn_find_id = btn_id_base+__LINE__;
int btn_close_id = btn_id_base+__LINE__;

void GuiFindDlg::CreateGUI()
{
    wxString default_root(getenv("HOME"));
#if wxCHECK_VERSION(3, 1, 0)
    default_root = wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Downloads);
#endif

    m_btnClose = new wxButton(this, btn_close_id, _T("&Close"));
    m_btnFind = new wxButton(this, btn_find_id, _T("Find"));
    m_btnCancel = new wxButton(this, btn_cancel_id, _T("Cancel"));
    m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler( GuiFindDlg::OnButton ), NULL, this );
    m_btnFind->Connect(btn_find_id, wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnFind), NULL, this);
    m_btnClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnButton), NULL, this);

    m_btnChooseRoot = new wxButton(this, wxID_ANY, _T("Root..."));
    m_btnChooseRoot->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnChooseRoot), NULL, this);

    m_root_path_tc = new wxTextCtrl(this, wxID_ANY, default_root);

    m_result_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
    		wxSize{200,100}, wxTE_READONLY | wxTE_MULTILINE); // crash on Mac without wxTE_READONLY
    m_filename_pattern_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    m_filename_pattern_exclude = new wxTextCtrl(this, wxID_ANY, wxEmptyString);

    wxGridBagSizer *gbs = new wxGridBagSizer();
    int col = 0;
    int row = 0;
    int col_span = 3;
    gbs->Add(new wxStaticText(this, wxID_ANY, _T("File Name")),
            wxGBPosition(row,col), wxGBSpan(1, 1),
            wxALIGN_CENTER | wxALL);
    col++;
    gbs->Add(m_filename_pattern_tc, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL, 5);
    row++;
    col = 0;
    gbs->Add(new wxStaticText(this, wxID_ANY, _T("Exclude File Name")),
            wxGBPosition(row,col), wxGBSpan(1, 1),
            wxALIGN_CENTER | wxALL);
	col++;
	gbs->Add(m_filename_pattern_exclude, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL, 5);

    row++;
    col = 0;
    gbs->Add(m_btnChooseRoot, wxGBPosition(row, col));
    col++;
    gbs->Add(m_root_path_tc, wxGBPosition(row, col), wxGBSpan(1, col_span),
             wxEXPAND | wxHORIZONTAL);
    col = 0;
    row++;
    gbs->Add(m_result_tc, wxGBPosition(row, col), wxGBSpan(1, col_span+1),
             wxEXPAND | wxHORIZONTAL | wxVERTICAL);
    gbs->AddGrowableRow(row);

    row++;
    col = 0;
    gbs->Add(m_btnFind, wxGBPosition(row, col));
    col++;
    gbs->Add(m_btnCancel, wxGBPosition(row, col));
    col++;
    gbs->Add(m_btnClose, wxGBPosition(row, col));

    gbs->AddGrowableCol(col_span);

    this->SetSizer(gbs);
    gbs->SetSizeHints(this);
    gbs->Fit(this);
}

GuiFindDlg::GuiFindDlg()
    : wxFrame(NULL, wxID_ANY, wxString::Format(_T("Find file by name\t PID:%lu"), wxGetProcessId()),
    		wxDefaultPosition, wxSize(800, 600)
    		)
{
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GuiFindDlg::OnClose) );
	m_pTimer = new wxTimer(this,TIMER_ID);
    CreateGUI();
    m_log.open("debug.txt");
    m_wnd_closed = false;
}

GuiFindDlg::~GuiFindDlg() {
	m_wnd_closed = true;
}
void GuiFindDlg::OnChooseRoot(wxCommandEvent &evt)
{
    wxString dirHome;
    wxGetHomeDir(&dirHome);
    wxDirDialog dialog(this, _T("Choose root path"), dirHome,
    		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        m_root_path_tc->SetValue(dialog.GetPath());
    }
}
void GuiFindDlg::OnButton(wxCommandEvent & event)
{
	if (event.GetEventObject()==m_btnClose)
	{
		Close(true);
		return;
	}
	if (event.GetEventObject()==m_btnCancel)
	{
		wxGetApp().m_find_handler->Stop(true);
		AppendResult(_T("thread stopped"));
//		wxLogMessage(_T("thread should be stopped"));
		return;
	}
//	event.Skip();
}

void GuiFindDlg::AppendResult(wxString msg)
{// http://wiki.wxwidgets.org/Inter-Thread_and_Inter-Process_communication
	// http://wiki.wxwidgets.org/Custom_Events
	{
	boost::mutex::scoped_lock lk(wxGetApp().m_result_mx);
	if(m_wnd_closed)
		return;
	wxGetApp().m_result_list.push_back(msg);
	}

	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__
			<< " " << msg.utf8_str()
			<< ", result list size " << wxGetApp().m_result_list.size()
			<< std::endl;
	m_log.flush();
	wxCustomEvent event;
	wxPostEvent( this, event );
}

void GuiFindDlg::OnAppendResult(wxCustomEvent & evt)
{
	boost::mutex::scoped_lock lk(wxGetApp().m_result_mx);
	for (std::list<wxString>::iterator it=wxGetApp().m_result_list.begin();
			it!=wxGetApp().m_result_list.end();
			it++)
	m_result_tc->AppendText((*it)+_T("\n"));
	wxGetApp().m_result_list.clear();
	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__ << std::endl;
	m_log.flush();
}
void GuiFindDlg::OnUpdateResult(wxCommandEvent &aevt)
{
	wxCustomEvent evt;
	OnAppendResult(evt);
}
void GuiFindDlg::OnFind(wxCommandEvent &evt)
{
	wxGetApp().findthreadcount++;
	m_pTimer->Start(30);
	Connect(m_pTimer->GetId(),wxEVT_TIMER,wxTimerEventHandler( GuiFindDlg::OnTimerTimeout ), NULL, this );

	m_result_tc->SetValue(wxEmptyString);
	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__ << std::endl;
	m_log.flush();
	wxGetApp().SetProperty(ToStdWstring("m_filename_pattern_tc"), m_filename_pattern_tc->GetValue().ToStdWstring());
//	wxGetApp().SetProperty(ToStdWstring("m_content_pattern_tc"), m_content_pattern_tc->GetValue().ToStdWstring());
	wxGetApp().SetProperty(ToStdWstring("m_root_path_tc"), m_root_path_tc->GetValue().ToStdWstring());
	wxGetApp().SetProperty(wxString("m_filename_pattern_exclude").ToStdWstring(), m_filename_pattern_exclude->GetValue().ToStdWstring());

	wxGetApp().StartFind();
}

void GuiFindDlg::OnClose(wxCloseEvent& event) {
	{
		boost::mutex::scoped_lock lk(wxGetApp().m_result_mx);
		m_wnd_closed = true;
	}
//	std::cout << __func__ << std::endl;
    if ( event.CanVeto() )
    {
    }
    Destroy();  // you may also do:  event.Skip();
                // since the default event handler does call Destroy(), too
}

void GuiFindDlg::OnTimerTimeout(wxTimerEvent& event) {
	if(wxGetApp().findthreadcount.load()<1) {
		m_pTimer->Stop();
	} else {
		wxGetApp().needrefresh.exchange(true);
	}
}
