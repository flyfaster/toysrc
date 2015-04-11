#include "guicopy_include.h"
#include "RenamePage.h"
#include "mydlg.h"
#include "mainapp.h"

BEGIN_EVENT_TABLE(RenamePage, wxPanel)
EVT_BUTTON(wxID_ANY, RenamePage::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<RenamePage>
{
	wxTextCtrl* m_directory;
	wxTextCtrl* m_find_pattern;
	wxTextCtrl* m_rename_pattern;
	wxButton* m_preview;
	wxButton* m_choose_directory;
	wxGrid* m_preview_grid;
	void RedirectPythonStdIo( boost::python::object main_namespace );
	void OnOK();
	void OnCancel();
	void OnPreview();
	void CreateUI(wxWindow* parent);
	bool PreviewResult(const tchar_string& src_file_name, const tchar_string& renamed_file_name);
	bool Rename(const tchar_string& src_file_name, const tchar_string& renamed_file_name);
	int row;
	typedef Loki::ImplOf<RenamePage> Type;
	typedef  bool (Type::*rename_mem_func_type)(const tchar_string& src_file_name, const tchar_string& renamed_file_name);
	void Process(rename_mem_func_type process_fun);
};
typedef Loki::ImplOf<RenamePage> PrivateRenamePage;
void Loki::ImplOf<RenamePage>::Process( PrivateRenamePage::rename_mem_func_type process_fun )
{
	m_preview_grid->ClearGrid();
	row = 0;
	using namespace boost::filesystem;
	tchar_string dir_path = m_directory->GetValue();
	tchar_string pattern_string = m_find_pattern->GetValue();
	tchar_string rename_pattern = m_rename_pattern->GetValue();
	boost::basic_regex<TCHAR> regex_find_pattern;
	regex_find_pattern.assign(pattern_string);
	std::basic_string<TCHAR> rename_result;
    tpath source_path_entry(dir_path.c_str(), boost::filesystem::native);
	if ( !exists( dir_path ) ) return ;
	if (boost::filesystem::is_directory(source_path_entry)==false)
	{
		return;
	}

	tdirectory_iterator end_itr; // default construction yields past-the-end
	for ( tdirectory_iterator itr( dir_path );
		itr != end_itr;
		++itr )
	{
		if ( is_directory(itr->status())==false )
		{
			tchar_string filename = itr->filename();//itr->path().directory_string();
			rename_result = boost::regex_replace(
				filename, 
				regex_find_pattern,
				rename_pattern
				, boost::match_default | boost::format_all);
			if (rename_result==filename)
				continue;
			if (((*this).*process_fun)(filename, rename_result)==false)
				break;
			row++;
		}
	}
    wxLogMessage(_T("%s processed %d files"), wxString::FromAscii(__FUNCTION__), row);
}
bool Loki::ImplOf<RenamePage>::Rename( const tchar_string& src_file_name, 
                                      const tchar_string& rename_result )
{
    boost::filesystem::rename(tpath(src_file_name, boost::filesystem::native),
        tpath(rename_result, boost::filesystem::native));
    return true;
	//int ret = rename(src_file_name.c_str(),
	//	rename_result.c_str());
	//if (row>=m_preview_grid->GetRows())
	//	return true;
	//m_preview_grid->SetCellValue(src_file_name, row, 0);
	//if (ret !=0 )
	//{
	//	// rename failed
	//	std::basic_stringstream<TCHAR> ss;
	//	ss << _T("rename to ") << rename_result << _T(" return ") << ret;
	//	unsigned long ec;
	//	if (_get_doserrno(&ec)==0)
	//		ss << _T(", doserrno = ") << ec;
	//	ss << _T(", _sys_nerr = ") << _sys_nerr;
	//	m_preview_grid->SetCellValue(ss.str(), row, 1);
	//}
	//else
	//{
	//	m_preview_grid->SetCellValue(rename_result, row, 1);
	//}
	//return true;
}
bool Loki::ImplOf<RenamePage>::PreviewResult( const tchar_string& filename, 
                                             const tchar_string& rename_result )
{
	if (row>=m_preview_grid->GetRows())
		return false;
	m_preview_grid->SetCellValue(filename, row, 0);
	m_preview_grid->SetCellValue(rename_result, row, 1);
	return true;
}
void Loki::ImplOf<RenamePage>::OnPreview()
{
	Process(&PrivateRenamePage::PreviewResult);

}
void Loki::ImplOf<RenamePage>::CreateUI(wxWindow* parent)
{
	wxGridBagSizer* sizer = new wxGridBagSizer();
	m_choose_directory = new wxButton(parent, wxID_ANY, _T("Directory..."));
	m_directory = new wxTextCtrl(parent, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	m_find_pattern = new wxTextCtrl(parent, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	m_rename_pattern = new wxTextCtrl(parent, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	m_preview = new wxButton(parent, wxID_ANY, _T("Preview"));

	m_preview_grid = new wxGrid(parent,
		-1,
		wxPoint( 0, 0 ),
		wxDefaultSize);
	m_preview_grid->CreateGrid( 20, 2 );
	m_preview_grid->SetColLabelValue(0, _T("Source file name"));
	m_preview_grid->SetColLabelValue(1, _T("Rename to"));
	int column_width = 280;
	m_preview_grid->SetColumnWidth(0, column_width);
	m_preview_grid->SetColumnWidth(1, column_width);
	m_preview_grid->SetRowLabelSize(40);
	int row = 0;
	int column = 0;
	sizer->Add(m_choose_directory, wxGBPosition(row, column));
	column++;
	sizer->Add(m_directory, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND | wxRIGHT);
	row++;
	column = 0;
	sizer->Add(new wxStaticText(parent, wxID_ANY, _T("File name patterns")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_find_pattern, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND | wxRIGHT);
	row++;
	column = 0;
	sizer->Add(new wxStaticText(parent, wxID_ANY, _T("Rename patterns")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_rename_pattern, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND | wxRIGHT);
	row++;
	column = 0;
	sizer->Add(m_preview, wxGBPosition(row, column));
	column++;
	sizer->Add(m_preview_grid, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND | wxRIGHT|wxBOTTOM|wxTOP|wxLEFT);
	sizer->AddGrowableCol(1);
	sizer->AddGrowableRow(row);

	parent->SetSizer(sizer);
	sizer->SetSizeHints(parent);
	sizer->Fit(parent);

	m_directory->SetValue(wxGetApp().ModuleDirectory ) ;
	m_find_pattern->SetValue( _T("([^.]*)(\\.obj)") );
	m_rename_pattern->SetValue( _T("aa(\\1)(\\2)") );
}
void Loki::ImplOf<RenamePage>::OnCancel()
{

}

void Loki::ImplOf<RenamePage>::OnOK()
{
	Process(&Loki::ImplOf<RenamePage>::Rename);

}

RenamePage::RenamePage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	d->CreateUI(this);
}

RenamePage::~RenamePage(void)
{
}

void RenamePage::OnButton( wxCommandEvent& ev )
{
	wxButton* active_button = dynamic_cast<wxButton*>(ev.GetEventObject());
	if (active_button)
	{
		if (active_button->GetLabelText()==wxT("OK"))
		{
			d->OnOK();
		}
		if (active_button->GetLabelText()==wxT("Cancel"))
		{
			d->OnCancel();
		}
		if(ev.GetEventObject()==d->m_choose_directory)
		{
			PathUtilDlg::ChooseDirectory( &SingleThreadsCol::Instance(), 
				_T("Choose working directory"), 
				d->m_directory);
		}
		if (ev.GetEventObject()==d->m_preview)
			d->OnPreview();
	}

}