// FindPage.cpp: implementation of the FindPage class.
//
//////////////////////////////////////////////////////////////////////
#include "guicopy_include.h"
#include "mydlg.h"
#include "mainapp.h"
#include "FindPage.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DEFINE_EVENT_TYPE(wxEVT_MY_EVENT)


BEGIN_EVENT_TABLE(FindPage, wxPanel)
EVT_BUTTON(wxID_ANY, FindPage::OnButton)
EVT_COMMAND  (wxID_ANY, wxEVT_MY_EVENT, FindPage::OnFindDoneEvent)
END_EVENT_TABLE()




class find_file_thread
{
public:
    volatile bool m_stoprequested;
    boost::mutex m_mutex;
    boost::thread m_thread;
    FindPage* m_find_page;
    bool m_started;
    void do_work()
    {
        try
        {
            wxString msg; msg.Printf(_T("thread %d start"), GetCurrentThreadId()); OutputDebugString(msg);
            int iteration = 0;
            m_find_page->FindFile();
            msg.Printf(_T("thread %d end"), GetCurrentThreadId()); OutputDebugString(msg);
        }
        catch (const std::exception& e)
        {
            m_find_page->PostDoneEvent( e.what());
        }
    }

    find_file_thread()
        : m_stoprequested(false)
    {
        m_started = false;
    }

    void start()
    {       
        m_started = true;
        m_thread = boost::thread(boost::bind(&find_file_thread::do_work, this));	
    }

    void stop()
    {
        if (m_started==false)
            return;
        m_started = false;
        m_stoprequested = true;
        m_thread.join();         
    }

    ~find_file_thread()
    {
         stop();
    }	
};


FindPage::FindPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
    m_cancel  = false;
    m_source_dirs = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_find_results = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_find_patterns = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_content_pattern = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    m_regex_content = new wxCheckBox(this, wxID_ANY, _T("Regex content pattern"));

    //m_choose_source_dir = new wxButton(this, wxID_ANY, _T("Choose source..."));
    m_copy_results = new wxButton(this, wxID_ANY, _T("Copy what"));
    m_clear_results = new wxButton(this, wxID_ANY, _T("Clear results"));
	m_copy_results->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindPage::OnCopyResultToClipboard), NULL, this);
    m_clear_results->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindPage::OnClearResults), NULL, this);
	m_source_type = new wxComboBox(this, wxID_ANY);
	m_source_type->AppendString(wxT("Content"));
	m_source_type->AppendString(wxT("File name"));
	m_source_type->AppendString(wxT("File name pattern"));
	m_source_type->AppendString(wxT("Directory name"));
	m_source_type->AppendString(wxT("Directory name pattern"));
	m_source_type->SetSelection(0);

    m_use_regex= new wxCheckBox(this, wxID_ANY, wxT("Use regex"));
    int row = 0;
    int col = 0;
    wxGridBagSizer* sizer = new wxGridBagSizer();
    //sizer->Add(m_choose_source_dir, wxGBPosition(row, col));
	sizer->Add(m_source_type, wxGBPosition(row, col));
    col++;
    sizer->Add(m_source_dirs, wxGBPosition(row, col), wxGBSpan(1, 2), wxEXPAND | wxRIGHT | wxVERTICAL);
    sizer->AddGrowableRow(row);
    row++;
    col = 0;
    sizer->Add(new wxStaticText(this, wxID_ANY, _T("File name patterns")), wxGBPosition(row, col));
    col++;
    sizer->Add(m_find_patterns, wxGBPosition(row, col), wxGBSpan(2, 1), wxEXPAND | wxRIGHT);
    col++;
    sizer->Add(m_regex_content, wxGBPosition(row, col));
    row++;
    col = 0;
    sizer->Add(m_use_regex, wxGBPosition(row, col));
    sizer->Add(m_content_pattern, wxGBPosition(row, col+2), wxGBSpan(1, 1), wxEXPAND | wxRIGHT);
    row++;
    col=0;
    sizer->Add(m_copy_results, wxGBPosition(row, col));
    col++;
    sizer->Add(m_find_results, wxGBPosition(row, col), wxGBSpan(2, 2), wxEXPAND | wxRIGHT|wxVERTICAL);
    row++;
    col=0;
    sizer->Add(m_clear_results, wxGBPosition(row, col));

    sizer->AddGrowableCol(sizer->GetCols()-1);
    sizer->AddGrowableCol(1);
    sizer->AddGrowableRow(row);
    this->SetSizer(sizer);
    sizer->SetSizeHints(this);
    sizer->Fit(this);

    m_source_dirs->SetDropTarget(new DropFileEdit(m_source_dirs));

}

FindPage::~FindPage()
{

}

wxXmlNode* FindPage::GetNode()
{
    wxXmlNode* page_node = new wxXmlNode(wxXML_ELEMENT_NODE, GetName());

    wxXmlNode* copy_source = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("SourceDirectory"));
    copy_source->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_source_dirs->GetValue()));

    page_node->AddChild(copy_source);
    wxXmlNode* copy_dst = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("FindPatterns"));
    copy_dst->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_find_patterns->GetValue()));
    page_node->AddChild(copy_dst);

    //wxXmlNode* flag_node = new wxXmlNode(wxXML_ELEMENT_NODE, _T("Results"));
    //page_node->AddChild(flag_node);
    //flag_node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("unused_name"), m_find_results->GetValue()));
    return page_node;
}

void FindPage::Init( wxXmlNode * child )
{
    wxXmlNode* page_child = child->GetChildren();
    while (page_child)
    {
        // process text enclosed by <tag1></tag1>
        tchar_string node_name = page_child->GetName().c_str();
        tchar_string content = page_child->GetNodeContent().c_str();

        if (node_name == wxT("SourceDirectory"))
        {
            m_source_dirs->SetValue(content);
        }
        else
            if (node_name == wxT("FindPatterns"))
            {
                m_find_patterns->SetValue(content);
            }
            else if(node_name==_T("Results"))
            {
                m_find_results->SetValue(content);
            }
            page_child = page_child->GetNext();
    }
}

wxString FindPage::GetName()
{
    return wxT("FindPage");
}

void FindPage::OnButton( wxCommandEvent& eventObj )
{
	wxButton* active_button = dynamic_cast<wxButton*>(eventObj.GetEventObject());
	if (active_button)
	{
		if (active_button->GetLabelText()==wxT("OK"))
		{
			OnOK();
			return;
		}
        if (active_button->GetLabelText()==wxT("Cancel"))
        {
            OnCancel();
        }
	}

    if (active_button==m_choose_source_dir)
    {
        PathUtilDlg::ChooseDirectory(this, _T("Choose directories to search"), m_source_dirs);
    }

    eventObj.Skip();
}

void FindPage::OnOK(void)
{
    PathUtilDlg* mainwnd = dynamic_cast<PathUtilDlg*>(wxGetApp().GetTopWindow());
    path_list.clear();
    tstringstream tss;
    tss << (LPCTSTR)m_source_dirs->GetValue();
    tchar_string oneline;
    while(std::getline(tss, oneline))
    {
        if (oneline.empty())
            continue;
        tpath one_path(oneline, boost::filesystem::native);
        if (!boost::filesystem::exists(one_path))
        {
            mainwnd->Dump( oneline + wxT(" does not exist.") );
            continue;
        }
        path_list.push_back(one_path);  mainwnd->Dump(oneline);
    }
    tss.str(_T("")); tss.seekg(0);
    //tchar_string file_name_pattern = m_find_patterns->GetValue();
    ParsePatterList();
    //FindFile();
    m_find_thread = boost::shared_ptr<find_file_thread>(new find_file_thread);
    m_find_thread->m_find_page = this;
    m_find_thread->start();
    return;
	// User hit "OK" button, start finding...
	// capture string find
	tchar_string pattern_string = m_find_patterns->GetValue();
	tchar_string source_string = m_source_dirs->GetValue();
	//boost::basic_regex<wxChar> regex_engine(pattern_string);
	//boost::match_results<std::basic_string<TCHAR>::const_iterator> what;
	//std::basic_stringstream<wxChar> os;
	tregex regex_engine/*(pattern_string)*/;
	regex_engine.assign(pattern_string);
	tmatch_results what;
    //boost::match_results<const TCHAR* > what;

	tstringstream os;
	unsigned i=0, j=0;
    if (boost::regex_match
//         <
//         std::char_traits<TCHAR>, 
//         std::allocator<TCHAR>, 
//         std::allocator<TCHAR>, 
//         TCHAR,
//         std::char_traits<TCHAR>
//     >
    ( source_string,
		what,
		regex_engine/*, boost::match_extra*/))
	{
		//os << "** Match found **\n   Sub-Expressions:\n";
		//for(i = 0; i < what.size(); ++i)
		//	os << wxT("      $") << i << wxT(" = \"") << what[i] << wxT("\"\n");
		//os << wxT("   Captures:\n");
		//for(i = 0; i < what.size(); ++i)
		//{
		//	os << wxT("      $") << i << wxT(" = {");
		//	for(j = 0; j < what.captures(i).size(); ++j)
		//	{
		//		if(j)
		//			os << wxT(", ");
		//		else
		//			os << wxT(" ");
		//		os << wxT("\"") << what.captures(i)[j] << wxT("\"");
		//	}
		//	os << wxT(" }\n");
		//}
	}
	m_find_results->SetValue(os.str());
}

void FindPage::FindFile()
{
    // input    source directory 
    // input    file name patter
    // output   file path that matches search criteria
    m_cancel= false;
    m_result_str.clear();
    tstringstream tss;
    int match_count = 0;
    int checked_count = 0;
    tdirectory_iterator end_itr; // default construction yields past-the-end
    while(path_list.size())
    {
        if (m_cancel)
            break;
        tpath one_path = path_list.front();
        path_list.pop_front();  
        for ( tdirectory_iterator itr( one_path );
            itr != end_itr;
            ++itr )
        {
            if (m_cancel)
                break;
            if ( is_directory(itr->path()) )
            {
                path_list.push_front(itr->path());
            }
            else 
            {
               // check if file name match pattern
                tchar_string dir_str = itr->path().directory_string();  
                if ( IsInterestedFile(itr->path()) )
                {
                    // file name matched
                    tss << dir_str << std::endl;
                    match_count++;
                }
                else
                {
                    checked_count++;
                }
            }
        }
    }
    m_result_str = tss.str();
    tss.str(_T(""));
    tss <<  (m_cancel? _T("Operation cancelled, "):_T("Operation completed, "));
    tss << _T("match count ") << match_count << _T(", checked ") << checked_count << _T(" files.") << std::endl;
    PostDoneEvent( tss.str());
}

void FindPage::OnCopyResultToClipboard( wxCommandEvent& evt )
{
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(m_find_results->GetValue()));
        wxTheClipboard->Close();
    }
    else
    {
        wxMessageBox(_T("Failed to open clipboard"));
    }
}

bool FindPage::IsInterestedFile( const tpath& file_path )
{
    tchar_string dir_str = file_path.directory_string();
    for (std::list<tchar_string>::const_iterator it=m_filename_pattern_list.begin();
        it!=m_filename_pattern_list.end();
        it++)
    {
        if (boost::ifind_first(dir_str, *it))
            return true;
    }
    return false;
}

void FindPage::ParsePatterList()
{
    typedef boost::tokenizer<boost::char_separator<TCHAR>,
        tchar_string::const_iterator,
        tchar_string > tokenizer;
    boost::char_separator<TCHAR> sep( _T("\n,; \t"));
    tchar_string str = m_find_patterns->GetValue();;
    tokenizer tokens(str, sep);
    tokenizer::iterator tok_iter = tokens.begin();
    tchar_string one_line;
    m_filename_pattern_list.clear();
    while(tok_iter!=tokens.end())
    {
        one_line = *tok_iter++;
        if ( std::find(  m_filename_pattern_list.begin(), 
            m_filename_pattern_list.end(),
            one_line) == m_filename_pattern_list.end())
            m_filename_pattern_list.push_back(one_line);
    }
}

void FindPage::OnClearResults( wxCommandEvent& evt )
{
    m_find_results->SetValue(wxEmptyString);
}

void FindPage::OnFindDoneEvent( wxCommandEvent &evt )
{
    m_find_results->SetValue(m_result_str);
    m_result_str.clear();
    PathUtilDlg* mainwnd = dynamic_cast<PathUtilDlg*>(wxGetApp().GetTopWindow());
    mainwnd->Dump( (LPCTSTR)evt.GetString() );
}

void FindPage::PostDoneEvent( wxString msg )
{
    wxCommandEvent event( wxEVT_MY_EVENT, GetId() );
    event.SetEventObject( this );
    event.SetString( msg );
    wxPostEvent(this, event);
}

void FindPage::OnCancel()
{
    m_cancel = true;
    if (m_find_thread)
        m_find_thread->stop();
    OutputDebugStringA(__FUNCTION__);
}
