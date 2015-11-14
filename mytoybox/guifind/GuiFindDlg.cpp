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

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include "wx/dirdlg.h"
#include <boost/thread/mutex.hpp>
#include "GuiFindDlg.h"
#include "GuiFindApp.h"
#include "FindThread.h"
int WORKER_EVENT = 111222;

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
int btn_id_base = 7000;
int btn_cancel_id = btn_id_base+__LINE__;
int btn_find_id = btn_id_base+__LINE__;
int btn_close_id = btn_id_base+__LINE__;
IMPLEMENT_DYNAMIC_CLASS( GuiFindDlg, wxFrame )
void GuiFindDlg::CreateGUI()
{
    m_btnClose = new wxButton(this, btn_close_id, _T("&Close"));
    m_btnFind = new wxButton(this, btn_find_id, _T("Find"));
    m_btnCancel = new wxButton(this, btn_cancel_id, _T("Cancel"));
    m_btnChooseRoot = new wxButton(this, wxID_ANY, _T("Root..."));
    m_content_pattern_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
    		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_result_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
    		wxDefaultSize, wxTE_MULTILINE);
    wxGridBagSizer *gbs = new wxGridBagSizer();
    m_filename_pattern_tc = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler( GuiFindDlg::OnButton ), NULL, this );
    m_btnFind->Connect(btn_find_id, wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnFind), NULL, this);
    m_btnClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnButton), NULL, this);
    m_btnChooseRoot->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
    		wxCommandEventHandler(GuiFindDlg::OnChooseRoot), NULL, this);
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
    gbs->Add(new wxStaticText(this, wxID_ANY, _T("Content")),
            wxGBPosition(row,col), wxGBSpan(1, 1),
            wxALIGN_CENTER | wxALL);
    col++;
    gbs->Add(m_content_pattern_tc, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL, 5);
    row++;
    col = 0;
    gbs->Add(m_btnChooseRoot, wxGBPosition(row, col));
    col++;
    m_root_path_tc = new wxTextCtrl(this, wxID_ANY, _T("/"),
                                        wxDefaultPosition,
                                        wxSize(150,wxDefaultCoord));
    gbs->Add(m_root_path_tc, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL);
    col = 0;
    row++;
    gbs->Add(m_result_tc, wxGBPosition(row, col), wxGBSpan(1, col_span + 1), wxEXPAND | wxHORIZONTAL | wxVERTICAL);
    row++;
    col = 0;
    gbs->Add(m_btnFind, wxGBPosition(row, col));
    col++;
    gbs->Add(m_btnCancel, wxGBPosition(row, col));
    col++;
    gbs->Add(m_btnClose, wxGBPosition(row, col));

    gbs->AddGrowableCol(col_span);
    gbs->AddGrowableRow(3);

    this->SetSizer(gbs);
    gbs->SetSizeHints(this);
    gbs->Fit(this);
    m_log.open("debug.txt");
}

GuiFindDlg::GuiFindDlg()
    : wxFrame(NULL, wxID_ANY, wxString(_T("Find file by name or content")),
    		wxDefaultPosition, wxSize(800, 600)
    		)
{
    CreateGUI();
}
GuiFindDlg::GuiFindDlg(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxString(_T("Find file by name or content")),
    		wxDefaultPosition, wxSize(800, 600)
    		)
{
CreateGUI();
}

GuiFindDlg::~GuiFindDlg() {

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
	boost::mutex::scoped_lock lk(m_result_mx);
	m_result_list.push_back(msg);
	}
	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__
			<< " " << msg.utf8_str()
			<< ", result list size " << m_result_list.size()
			<< std::endl;
	m_log.flush();
	wxCustomEvent event;
	wxPostEvent( this, event );
}

void GuiFindDlg::OnAppendResult(wxCustomEvent & evt)
{
	boost::mutex::scoped_lock lk(m_result_mx);
	for (std::list<wxString>::iterator it=m_result_list.begin();
			it!=m_result_list.end();
			it++)
	m_result_tc->AppendText((*it)+_T("\n"));
	m_result_list.clear();
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
	time_t now;
	time(&now);
	m_log <<ctime(&now) << " "<< __FUNCTION__ << std::endl;
	m_log.flush();
	new boost::thread(dummy_thread_func);
}



