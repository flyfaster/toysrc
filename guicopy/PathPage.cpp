#include "guicopy_include.h"
#include "mydlg.h"
#include "PathPage.h"


BEGIN_EVENT_TABLE(PathPage, wxPanel)
EVT_BUTTON(wxID_ANY, PathPage::OnButton)
END_EVENT_TABLE()


PathPage::PathPage(wxWindow* parent): wxPanel(parent, wxID_ANY)
{
	m_path_relative_path = new wxTextCtrl(this,wxID_ANY);
	m_path_choose_target = new wxButton(this, wxID_ANY, _T("Target Directory..."));
	m_path_choose_current = new wxButton(this, wxID_ANY, _T("Current Directory..."));
	m_path_target_path = new wxTextCtrl(this, wxID_ANY);
	m_path_current_path_input = new wxTextCtrl(this, wxID_ANY);
	m_path_target_path->SetDropTarget(new DropFileEdit(m_path_target_path));
	m_path_current_path_input->SetDropTarget(new DropFileEdit(m_path_current_path_input));
	m_cmd_path = new wxTextCtrl(this, wxID_ANY);
	m_cmd_path->SetDropTarget(new DropFileEdit(m_cmd_path));
	m_choose_cmd = new wxButton(this, wxID_ANY, _T("Choose command..."));
	m_cmd_path->SetValue(wxT("cmd.exe"));

	const int RIGHT_EXPAND = wxEXPAND | wxRIGHT;
	wxGridBagSizer* sizer = new wxGridBagSizer();
	int row = 0;
	int column = 0;
	sizer->Add(m_path_choose_current, wxGBPosition(row,column));
	column++;
	sizer->Add(m_path_current_path_input, wxGBPosition(row, column), wxDefaultSpan, RIGHT_EXPAND);
	row++;
	column = 0;
	sizer->Add(m_path_choose_target, wxGBPosition(row,column));
	column++;
	sizer->Add(m_path_target_path, wxGBPosition(row, column), wxDefaultSpan, RIGHT_EXPAND);


	row++;
	column = 0;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Relative path")), wxGBPosition(row,column));
	column++;
	sizer->Add(m_path_relative_path, wxGBPosition(row,column), wxDefaultSpan, RIGHT_EXPAND);

	row++;
	column = 0;
	sizer->Add(m_choose_cmd, wxGBPosition(row, column));
	column++;
	sizer->Add(m_cmd_path, wxGBPosition(row, column), wxDefaultSpan, RIGHT_EXPAND);
	sizer->AddGrowableCol(1);

	SetSizer(sizer);
	sizer->SetSizeHints(this);
	sizer->Fit(this);
}

PathPage::~PathPage(void)
{
}

void PathPage::OnOK()
{
	if(RunCmd())
		return;
	wxString current_dir = m_path_current_path_input->GetValue();
	wxString target_dir = m_path_target_path->GetValue();
	if (current_dir.length() && 0== wxStricmp(target_dir, current_dir))
	{
		m_path_relative_path->SetValue(_T(".\\"));
		return;
	}
	tstringlist cur_dir_que;
	tstringlist obj_dir_que;
	path_to_components_que((current_dir.c_str()), cur_dir_que);
	path_to_components_que((target_dir.c_str()), obj_dir_que);
	if(cur_dir_que.size()<=0 || obj_dir_que.size()<=0)
	{
		wxMessageBox(_T("Must set current directory and target directory!"));
		return;
	}
	//compare drive letter
	if(make_upper(cur_dir_que.front())!=make_upper(obj_dir_que.front()))
	{
		wxMessageBox(_T("Current dir and target dir must be on the same drive"));
		return;
	}
	cur_dir_que.pop_front();
	obj_dir_que.pop_front();
	while(cur_dir_que.size()>0 && obj_dir_que.size()>0)
	{
		if(make_upper(cur_dir_que.front())==make_upper(obj_dir_que.front()))
		{
			cur_dir_que.pop_front();
			obj_dir_que.pop_front();
		}
		else
			break;
	}
	//
	//TCHAR path_sep_buf[32];
	//strcpy(path_sep_buf, _T("\\"));
	TCHAR const path_sep_buf[]=_T("\\");
	tstringstream tss;
	if(cur_dir_que.size()==0&&obj_dir_que.size()==0)
	{
		tss<<_T(".")<<path_sep_buf;
		m_path_relative_path->SetValue(tss.str().c_str());
		return;
	}
	if(cur_dir_que.size()==0)
	{
		tss<<_T(".")<<path_sep_buf;
	}
	else
	{
		while(cur_dir_que.size()>0)
		{
			tss<<_T("..")<<path_sep_buf;
			cur_dir_que.pop_front();
		}
	}
	while(obj_dir_que.size()>0)
	{
		tchar_string s = obj_dir_que.front();
		obj_dir_que.pop_front();
		tss<<s<<path_sep_buf;
	}
	m_path_relative_path->SetValue(tss.str().c_str());
}

void PathPage::OnButton( wxCommandEvent& event )
{
	if(event.GetEventObject()==m_path_choose_target)
	{
		PathUtilDlg::ChooseDirectory(this, _T("Choose target directory"), m_path_target_path);
	}
	else if(event.GetEventObject()==m_path_choose_current)
	{
		PathUtilDlg::ChooseDirectory(this, _T("Choose current directory"), m_path_current_path_input);
	}
	else if(event.GetEventObject()==m_choose_cmd)
	{
		wxFileDialog* openFileDialog =
			new wxFileDialog( this, _T("Open file"), wxT(""), wxT(""), wxT("Any file(*.*)|*.*"),
			wxFD_OPEN, wxDefaultPosition);

		if ( openFileDialog->ShowModal() == wxID_OK )
		{
			m_cmd_path->SetValue(openFileDialog->GetFilename());
		}
	}
	else
	{
		event.Skip();
	}
}

wxString PathPage::GetName()
{
	return _T("PathPage");
}


void PathPage::path_to_components_que(LPCTSTR cur_dir_buf,tstringlist& qque)
{
	boost::char_separator<TCHAR> sep( _T("\\"));
	tchar_string str = cur_dir_buf;
	tokenizer tokens(str, sep);
	tokenizer::iterator tok_iter = tokens.begin();
	tchar_string group_element;
	while(tok_iter!=tokens.end())
	{
		group_element = *tok_iter;
		qque.push_back(group_element);
		tok_iter++;
	}
	if(!PathIsDirectory(cur_dir_buf) && qque.size()>1)
	{
		qque.pop_back();
	}
}

tchar_string PathPage::make_upper( tchar_string src )
{
	tchar_string sret=src;
	std::transform(src.begin(),src.end(),sret.begin(),::toupper);
	return sret;
}

bool PathPage::RunCmd()
{
	wxString current_dir = m_path_current_path_input->GetValue();
	wxString target_dir = m_path_target_path->GetValue();
	wxString cmd_path = m_cmd_path->GetValue();
	if (cmd_path.size() && current_dir.size() && (target_dir.size()==0))
	{
		STARTUPINFO start_info;
		memset(&start_info, 0, sizeof(start_info));
		start_info.cb = sizeof(start_info);

		PROCESS_INFORMATION proc_info;
		memset(&proc_info, 0, sizeof(proc_info));
		
		CreateProcess(NULL, 
			const_cast<TCHAR*>(cmd_path.wx_str()), 
			NULL,
			NULL,
			FALSE,
			CREATE_SUSPENDED,
			NULL,
			current_dir.wx_str(),
			&start_info,
			&proc_info
			);
		ResumeThread(proc_info.hThread);
		CloseHandle(proc_info.hThread);
		CloseHandle(proc_info.hProcess);
		return true;
	}
	return false;
}