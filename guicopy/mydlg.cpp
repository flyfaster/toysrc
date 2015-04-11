// -----------------------------------------------------------------------------
// File:             mydlg.cpp
// Description:
// Original author:  Onega Zhang
// Details:
//
// Notes:
//
// (C) Copyright 2006 - 2007, www.fruitfruit.com, All rights reserved.
// -----------------------------------------------------------------------------
#include "guicopy_include.h"

#include "mydlg.h"
#include "CopyPage.h"
#include "VerbosePage.h"
#include "CopyThread.h"
#include "DeleteThread.h"
#include "ConfigPage.h"
#include "FindPage.h"
#include "PythonPage.h"
#include "CrcPage.h"
#include "PathPage.h"
#include "CleanPage.h"
#include "CapturePage.h"
#include "RenamePage.h"
#include "Base64Page.h"
#include "AboutPage.h"
#include "mainapp.h"
#include <boost/algorithm/string.hpp>
PathUtilDlg::PathUtilDlg():
wxFrame(NULL, wxID_ANY, wxString(_T("Utils")), wxDefaultPosition,
         wxSize(800,600))
{
    m_host_thread_id = GetCurrentThreadId();
    // add menu
    wxMenuBar* menu_bar = new wxMenuBar();
    wxMenu* simple_menu = new wxMenu();
    simple_menu->Append(wxID_ABOUT, _T("About..."));
    simple_menu->Append(wxID_EXIT, _T("Exit..."));
    menu_bar->Append(simple_menu, _T("Application"));
    SetMenuBar(menu_bar);

    this->CreateStatusBar();
    m_path_page = NULL;
    m_clean_page = NULL;
    wxString text;
    text.Printf(wxT("This is text box %08x"), this);
    m_btnOK = new wxButton(this, wxID_ANY, _T("&OK"));
    m_btnCancel = new wxButton(this, wxID_ANY, _T("&Cancel"));
    m_btnExit = new wxButton(this, wxID_ANY, _T("&Exit"));

    m_book = new wxNotebook(this, wxID_ANY);

	m_path_page =  new PathPage(m_book);
	m_book->AddPage(m_path_page, m_path_page->GetName());
	m_clean_page = new CleanPage(m_book);
    m_book->AddPage(m_clean_page, _T("Clean"));
    m_copy_page = new CopyPage(m_book);
    m_book->AddPage(m_copy_page, _T("Copy"));
    m_verbose_page = new VerbosePage(m_book);
    m_book->AddPage(m_verbose_page, _T("Verbose"));
    m_config_page = new ConfigPage(m_book);
    m_book->AddPage(m_config_page, _T("Configuration"));
    m_find_page = new FindPage(m_book);
    m_book->AddPage(m_find_page, m_find_page->GetName());
	m_crc_page = new CrcPage(m_book);
	m_book->AddPage(m_crc_page, m_crc_page->GetName());
	//m_capture_page = new CapturePage(m_book);
	//m_book->AddPage(m_capture_page, wxT("CapturePage"));

	m_python_page = new PythonPage(m_book);
	m_book->AddPage(m_python_page, _T("Python"));

	m_rename_page = new RenamePage(m_book);
	m_book->AddPage(m_rename_page, _T("Rename"));
	m_book->SetSelection(9);
	
	m_base64_page = new Base64Page(m_book);
	m_book->AddPage(m_base64_page, _T("Base64"));

    AboutPage* m_about_page = new AboutPage(m_book);
    m_book->AddPage(m_about_page, _T("About"));

    wxGridBagSizer* sizer = new wxGridBagSizer();
    sizer->Add(m_book, wxGBPosition(0, 0), wxGBSpan(3, 3), wxALL | wxEXPAND );
    sizer->Add(m_btnOK, wxGBPosition(3, 0), wxDefaultSpan, wxALIGN_RIGHT);
    sizer->Add(m_btnCancel, wxGBPosition(3, 1), wxDefaultSpan, wxALIGN_CENTER);
    sizer->Add(m_btnExit, wxGBPosition(3, 2), wxDefaultSpan, wxALIGN_LEFT);
    sizer->AddGrowableRow(0);
	sizer->AddGrowableCol(0);
    sizer->AddGrowableCol(2);
//     sizer->Add(m_statusbar, wxGBPosition(4, 0), wxGBSpan(1, 4), wxEXPAND|wxHORIZONTAL);
    LoadConfiguration();
	SetSizer(sizer);
    sizer->SetSizeHints(this);
	//sizer->Fit(this);
    //SetAutoLayout(true);
	this->SetSizeHints(wxSize(800, 600));
	this->CenterOnParent(wxBOTH);
    m_btnOK->SetFocus();
    m_btnOK->SetDefault();

	bool reghotkey = RegisterHotKey(CapturePage::hotkey_id, wxMOD_CONTROL, '8');
	if (!reghotkey)
		wxLogError(_T("Fail to register hotkey"));

}



BEGIN_EVENT_TABLE(PathUtilDlg, wxFrame)
EVT_BUTTON(wxID_ANY, PathUtilDlg::OnButton)
EVT_CLOSE(PathUtilDlg::OnCloseWindow)
// EVT_INIT_DIALOG(PathUtilDlg::OnInitDialog)
EVT_KEY_DOWN(PathUtilDlg::OnKeyDown)
EVT_CHAR(PathUtilDlg::OnKeyDown)
EVT_KEY_UP(PathUtilDlg::OnKeyDown)
// EVT_MENU(wxID_CLOSE, PathUtilDlg::OnClose)
EVT_MENU(wxID_ABOUT, PathUtilDlg::OnAbout)
EVT_MENU(wxID_EXIT, PathUtilDlg::OnExit)
EVT_HOTKEY(CapturePage::hotkey_id, PathUtilDlg::OnHotkey)
END_EVENT_TABLE()

void PathUtilDlg::OnButton( wxCommandEvent& event )
{
	wxWindow * pcur = m_book->GetCurrentPage();
    if ( event.GetEventObject() == m_btnOK )
    {
        if (   pcur == m_clean_page 
			|| pcur == m_copy_page 
			|| pcur == m_find_page
			|| pcur == m_capture_page
			|| pcur == m_python_page
			|| pcur == m_rename_page
			|| pcur == m_verbose_page
			)
        {
            //pcur->OnOK();
			pcur->GetEventHandler()->ProcessEvent(event);
             event.Skip(false);
        }
        if (pcur == m_path_page)
        {
            m_path_page->OnOK();
        }
		if (pcur == m_crc_page)
		{
			m_crc_page->OnOK();
		}
    }
    else if(event.GetEventObject()==m_btnExit)
    {
        SaveConfiguration();
        Close();
        //EndModal(0);
    }
    else if(event.GetEventObject()==m_btnCancel)
    {
        if (pcur == m_find_page)
        {
            m_find_page->OnButton(event);
            return;
        }
		if (   pcur == m_python_page
			|| pcur == m_rename_page
			|| pcur == m_verbose_page
			|| pcur == m_base64_page
            || pcur == m_find_page
			)
		{
            //event.StopPropagation();
            //wxPostEvent(pcur, event);
            //event.Skip();
//			pcur->ProcessEvent(event);
		}
        while(m_threads.size())
        {
            wxCriticalSectionLocker locker(m_cs);
            if (m_threads.size())
            {
                wxThread* p = m_threads.front();
                m_threads.pop_front();
                p->Delete();
                OBaseThread* pbase = dynamic_cast<OBaseThread*>(p);
                if (pbase)
                {
                    pbase->m_cancel = true;
                }
            }
        }
    }
    else
    {
        event.Skip();
    }

}



void PathUtilDlg::ChooseDirectory(wxWindow* parent, const wxString title, wxTextCtrl* receiver )
{
    if (!receiver)
    {
        wxMessageBox(_T("NULL parameter"));
        wxExit();
    }
    wxString dirHome;
    wxGetHomeDir(&dirHome);
    wxDirDialog dialog(parent, title, dirHome,
        wxDD_DEFAULT_STYLE & ~wxDD_DIR_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK)
    {
        if (receiver->GetWindowStyleFlag() & wxTE_MULTILINE)
        {
            wxString old_value = receiver->GetValue();

            boost::char_separator<TCHAR> sep( _T("\n"));
            tchar_string str = old_value.c_str();
            tokenizer tokens(str, sep);
            tokenizer::iterator tok_iter = tokens.begin();
            tchar_string one_line;
            while(tok_iter!=tokens.end())
            {
                one_line = *tok_iter++;
                if (dialog.GetPath() == one_line)
                    return;
            }
            if (old_value.length())
            {
                receiver->AppendText(wxT("\r\n"));
            }
            receiver->AppendText(dialog.GetPath());
        }
        else
        {
            receiver->SetValue(dialog.GetPath());
        }
    }
}

void PathUtilDlg::RemoveThread( wxThread* p )
{
    wxCriticalSectionLocker locker(m_cs);
    int count = m_threads.size();
    m_threads.remove(p);
    int count2 = m_threads.size();
    if (count2!=count)
    {
        wxString msg;
        wxDateTime now = wxDateTime::Now();
        msg.Printf(_T("Remove thread %08x at %s"), p, now.FormatTime().wx_str());
        Dump(msg.wx_str());
    }
}

void PathUtilDlg::AddThread( wxThread* p )
{
    wxCriticalSectionLocker locker(m_cs);
    if (std::find(m_threads.begin(), m_threads.end(), p)==m_threads.end())
        m_threads.push_back(p);
}

void PathUtilDlg::Dump( std::wstring msg )
{
    bool lock = (m_host_thread_id!=GetCurrentThreadId());
    if (lock)
        wxMutexGuiEnter();
    this->SetStatusText( to_tchar_string(msg), 0);
    tchar_string s = to_tchar_string(msg);
    wxLogMessage(s.c_str());
    if (boost::ends_with(msg, "\r\n")==false)
    {
        s += wxT("\r\n");
    }
    OutputDebugString(s.c_str());
    if (lock)
        wxMutexGuiLeave();
}

void PathUtilDlg::Dump( std::string msg )
{	// wxPostEvent?
    OutputDebugStringA(msg.c_str());
    bool lock = (m_host_thread_id!=GetCurrentThreadId());
    if (lock)
        wxMutexGuiEnter();
    this->SetStatusText( to_tchar_string(msg), 0);
    wxString s = to_tchar_string(msg);
    wxLogMessage(s);
    if (boost::ends_with(msg, "\r\n")==false)
    {
        s += wxT("\r\n");
    }
    if (lock)
        wxMutexGuiLeave();
}

void PathUtilDlg::OnCloseWindow( wxCloseEvent& event )
{
    SaveConfiguration();
    Destroy();
}


void PathUtilDlg::SaveConfiguration()
{
    wxRemoveFile(m_config_page->GetFileName());
    wxXmlDocument doc;
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("PathUtil"));
    root->AddProperty(wxT("xmlns"), wxT("http://www.fruitfruit.com/OnegaZhang/PathUtil"));
    doc.SetRoot(root);

    // copy page
    root->AddChild(m_copy_page->GetNode());
    root->AddChild(m_find_page->GetNode());
    // delete page
	root->AddChild(m_clean_page->GetNode());

    doc.Save(m_config_page->GetFileName());
}

static wxString GetNodeContent(const wxXmlNode *node)
{
    const wxXmlNode *n = node;
    if (n == NULL) return wxEmptyString;
    n = n->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE && n->GetContent().length())
            return n->GetContent();
        n = n->GetNext();
    }

    return wxEmptyString;
}


void PathUtilDlg::LoadConfiguration()
{
//     return;
    tchar_string filename = m_config_page->GetFileName().c_str();
    if (PathUtilApp::IsDirExists(filename)==false)
    {
        wxString msg;
        msg.Printf(_T("%s not exixts"), m_config_page->GetFileName().c_str());
        Dump(msg.wx_str());
        return;
    }
    wxXmlDocument doc;
    if (!doc.Load(m_config_page->GetFileName()))
        return;
    if (doc.GetRoot()->GetName() != wxT("PathUtil"))
        return;

    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == m_copy_page->GetName())
        {
            m_copy_page->Init(child);
        }
        else
            if (child->GetName()==wxT("CleanPage"))
            {
				m_clean_page->Init(child);
            }
            else
                if (child->GetName()==m_find_page->GetName())
                {
                    m_find_page->Init(child);
                }
            child = child->GetNext();
    }
}

wxXmlNode* PathUtilDlg::GetNode( wxListBox* listbox, tchar_string group_name, tchar_string element_name )
{
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, group_name);
    wxArrayString exts = listbox->GetStrings();
    for(size_t j=0; j<exts.size(); j++)
    {
        tchar_string s = exts[j].c_str();
        wxXmlNode* ext = new wxXmlNode(wxXML_ELEMENT_NODE, element_name);
        ext->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("unused_dummy_name"), s));
        root->AddChild(ext);
    }
    return root;
}

void PathUtilDlg::LoadNode( wxListBox* listbox, wxXmlNode* node, tchar_string group_name, tchar_string element_name )
{
    tchar_string name = node->GetName().c_str();
    if (name!=group_name)
        return;
    wxXmlNode* child = node->GetChildren();
    while(child)
    {
        if (child->GetName()==element_name)
        {
            tchar_string input_extension = child->GetNodeContent().c_str();
            if (input_extension.length() && wxNOT_FOUND ==listbox->FindString(input_extension, false))
            {
                listbox->Append(input_extension);
            }
        }
        child = child->GetNext();
    }
}



void PathUtilDlg::DeleteSelections( wxListBox* listbox )
{
    wxArrayInt selections;
    listbox->GetSelections(selections);
    for (int i=selections.size()-1; i>=0; i--)
    {
        listbox->Delete(selections[i]);
    }

}

void PathUtilDlg::AddExtensions( wxListBox* listbox, tchar_string body_prompt, tchar_string catpion )
{
	wxTextEntryDialog dialog( &SingleThreadsCol::Instance(),
        body_prompt,
        catpion,
        _T(""),
        wxOK | wxCANCEL);
    if (dialog.ShowModal() == wxID_OK)
    {
        tchar_string str = dialog.GetValue().c_str();

        boost::char_separator<TCHAR> sep( _T(",; "));
        tokenizer tokens(str, sep);
        tokenizer::iterator tok_iter = tokens.begin();
        tchar_string group_element;
        while(tok_iter!=tokens.end())
        {
            group_element = *tok_iter;
            if (wxNOT_FOUND ==listbox->FindString(group_element, false))
            {
                listbox->Append(group_element);
            }
            tok_iter++;
        }
    }
}

void PathUtilDlg::OnKeyDown( wxKeyEvent& event )
{
    if (event.GetKeyCode()==WXK_DELETE)
    {
        wxWindow* current_page = m_book->GetCurrentPage();
        if (current_page == m_copy_page
			)
        {
            m_copy_page->OnKeyDown(event);
			return;
        }
        if (current_page == m_clean_page)
        {
            m_clean_page->OnDelete();
			return;
        }
		if (current_page == m_verbose_page)
		{
			m_verbose_page->OnKeyUp(event);
			return;
		}
    }
    event.Skip(true);
}

void PathUtilDlg::OnClose( wxCommandEvent& event )
{
    Close(true);
}

void PathUtilDlg::OnAbout( wxCommandEvent& eventobj )
{
    wxMessageBox(_("PathUtil, 20070805"));
}

void PathUtilDlg::OnExit( wxCommandEvent& eventobj )
{
	UnregisterHotKey(CapturePage::hotkey_id);
    Close(true);
    eventobj.Skip();
}

void PathUtilDlg::OnHotkey( wxKeyEvent& evt )
{
	Dump(__FUNCTION__);
//	m_capture_page->ProcessEvent(evt);
}

bool DropFileEdit::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
    if (!filenames.size())
        return false;

    if (m_pedit->GetWindowStyleFlag() & wxTE_MULTILINE )
    {
        if ( !m_pedit->GetValue().EndsWith(_T("\n")) )
        {
            m_pedit->AppendText(_T("\r\n"));
        }
        for (size_t i=0; i<filenames.size(); i++)
        m_pedit->AppendText(filenames[i]+_T("\r\n"));
        return true;
    }
    else
    {
        m_pedit->SetValue(filenames[0]);
        return true;
    }
    return false;
}
