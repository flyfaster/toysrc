#include "guicopy_include.h"
#include "mydlg.h"
#include "CleanPage.h"
#include "DeleteThread.h"
#include <boost/thread.hpp>

BEGIN_EVENT_TABLE(CleanPage, wxPanel)
EVT_BUTTON(wxID_ANY, CleanPage::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<CleanPage>
{
	wxButton* m_preview;
	wxListBox* m_preview_list;
	wxTextCtrl* m_del_root_path;
	wxListBox* m_del_ext_list;
	wxButton* m_del_add_ext;
	wxButton* m_del_ext;
	wxButton* m_del_select_root;
	boost::thread::id m_preview_thread_id;
	bool m_in_preview; // preview thread will set it to true. dtor can set it to false, 
	// so that preview thread knows it is time to stop
	boost::mutex m_lockable;
	boost::thread m_preview_thread;

	void CreateUI(wxWindow* control_container);
	void OnPreview();
	ImplOf();
	~ImplOf();
	void PreviewThreadProc();
};

Loki::ImplOf<CleanPage>::~ImplOf()
{
	
}

long ctor_count = 0;
void Loki::ImplOf<CleanPage>::PreviewThreadProc()
{
	{
		boost::mutex::scoped_lock locker(m_lockable);
	}
    wxLogMessage( wxT("%s, ctor count is %d"), wxString::FromAscii(__FUNCTION__), ctor_count);
	{
		boost::mutex::scoped_lock locker(m_lockable);
		m_in_preview = false;
	}

}

Loki::ImplOf<CleanPage>::ImplOf()
{
	m_in_preview = false;
	ctor_count++;
}

void Loki::ImplOf<CleanPage>::OnPreview()
{
	boost::mutex::scoped_lock locker(m_lockable);
	if (m_in_preview)
		return;
	m_in_preview = true;
	m_preview_thread = boost::thread(boost::bind(&Loki::ImplOf<CleanPage>::PreviewThreadProc, this));
}

void Loki::ImplOf<CleanPage>::CreateUI( wxWindow* control_container )
{
	m_preview = new wxButton(control_container, wxID_ANY, _T("&Preview"));
	m_preview_list = new wxListBox(control_container, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0
		, NULL, wxLB_SORT|wxLB_EXTENDED);
	wxGridBagSizer* sizer = new wxGridBagSizer();
	int row = 0;
	int column = 0;
	m_del_select_root = new wxButton(control_container, wxID_ANY, _T("&Root Path..."));
	sizer->Add( m_del_select_root, wxGBPosition(row,column));
	column++;
	m_del_root_path = new wxTextCtrl(control_container, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);

	sizer->Add(m_del_root_path, wxGBPosition(row, column), wxGBSpan(1,1), wxEXPAND | wxRIGHT);

	row++;
	column = 0;
	m_del_add_ext = new wxButton(control_container, wxID_ANY, _T("&Add..."));
	sizer->Add(m_del_add_ext, wxGBPosition(row, column));
	column++;
	m_del_ext_list = new wxListBox(control_container, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0
		, NULL, wxLB_SORT|wxLB_EXTENDED);
	sizer->Add(m_del_ext_list, wxGBPosition(row, column), wxGBSpan(2, 1), wxEXPAND|wxVERTICAL);
	row++;
	column=0;
	m_del_ext = new wxButton(control_container, wxID_ANY, _T("&Delete"));
	sizer->Add(m_del_ext, wxGBPosition(row, column));

	row++;
	column =0;
	sizer->Add(m_preview, wxGBPosition(row, column));
	column++;
	sizer->Add(m_preview_list, wxGBPosition(row, column), wxGBSpan(2, 1), wxEXPAND|wxRIGHT|wxVERTICAL);
	sizer->AddGrowableCol(1);
	sizer->AddGrowableRow(row);
	sizer->AddGrowableRow(0);

	wxArrayString ext_list;
	ext_list.Add(_T(".obj"));
	ext_list.Add(_T(".ilk"));
	m_del_ext_list->InsertItems(ext_list, 0);
	m_del_ext_list->Append(_T(".res"));
	m_del_ext_list->Append(_T(".idb"));
	m_del_ext_list->Append(_T(".pch"));
	m_del_ext_list->Append(_T(".sbr"));
	control_container->SetSizer(sizer);
	sizer->SetSizeHints(control_container);
	sizer->Fit(control_container);

	m_del_root_path->SetDropTarget(new DropFileEdit(m_del_root_path));
}

CleanPage::CleanPage(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
	d->CreateUI(this);
}

CleanPage::~CleanPage(void)
{
	d->m_in_preview = false;
	d->m_preview_thread.join();
}

void CleanPage::OnButton( wxCommandEvent& event )
{
	wxButton* active_button = dynamic_cast<wxButton*>(event.GetEventObject());
	if (active_button)
	{
		OutputDebugString(active_button->GetLabelText());
		wxString button_label = active_button->GetLabelText();
		if (button_label==wxT("OK"))
		{
			DeleteThread* del = new DeleteThread( d->m_del_root_path->GetValue(),
				d->m_del_ext_list->GetStrings());
			del->Run();
		}
		if (button_label==wxT("Cancel"))
		{

		}
	}
	if(event.GetEventObject()==d->m_del_select_root)
	{
		PathUtilDlg::ChooseDirectory(this, _T("Choose root path to clean"), d->m_del_root_path);
	}
	else if(event.GetEventObject()==d->m_del_add_ext)
	{
		PathUtilDlg::AddExtensions(d->m_del_ext_list,
			_T("Enter extension names (prefixed with dot) to list, extensions can be separated by , ; or whitespace"),
			_T("Please enter extension names"));
	}
	else if(event.GetEventObject()==d->m_del_ext)
	{
		PathUtilDlg::DeleteSelections(d->m_del_ext_list);
	}
	else if(event.GetEventObject()==d->m_preview)
	{
		d->OnPreview();
	}
}


wxXmlNode* CleanPage::GetNode()
{
	wxXmlNode* delete_root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("CleanPage"));

	// root path
	wxXmlNode* delete_dir = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("RootPath"));
	delete_dir->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT(""), d->m_del_root_path->GetValue()));
	delete_root->AddChild(delete_dir);

	delete_root->AddChild(PathUtilDlg::GetNode(d->m_del_ext_list, wxT("extensions"), wxT("extension")));
	return delete_root;
}

void CleanPage::Init( wxXmlNode * child )
{
	wxXmlNode* firstchild = child->GetChildren();
	while(firstchild)
	{
		tchar_string name = firstchild->GetName().c_str();
		if (name==wxT("RootPath"))
		{
			d->m_del_root_path->SetValue(firstchild->GetNodeContent());
		}
		else
		{
			PathUtilDlg::LoadNode(d->m_del_ext_list, firstchild, wxT("extensions"), wxT("extension"));
		}
		firstchild = firstchild->GetNext();
	}

}

void CleanPage::OnDelete()
{
	PathUtilDlg::DeleteSelections(d->m_del_ext_list);

}