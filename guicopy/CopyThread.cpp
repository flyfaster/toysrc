#include "guicopy_include.h"
#include "mainapp.h"
#include "mydlg.h"
#include "CopyThread.h"
#include <boost/algorithm/string.hpp>
CopyThread::FileTimeMap CopyThread::m_copied_files;
LPCTSTR COPIED_FILE_LIST_FILE = _T("copy_file_list.xml");
LPCTSTR copy_file_root_node = _T("CopiedFileList");
LPCTSTR copy_file_node = _T("file");
LPCTSTR copy_file_name = _T("name");
LPCTSTR copy_file_time = _T("lastmodify");
LPCTSTR copy_file_dst_time = _T("dst_time");


CopyThread::CopyThread( const wxString source,
                       const wxString destination, const wxArrayString& extension_list,
		        bool contained) : OBaseThread( extension_list)
{
    m_copy_count = 0;
	m_skip_count = 0;
	m_ignore_count = 0;
    m_contained = contained;
    Parse(source, m_path_collection);
    Parse(destination, m_destination_directory);
    LoadList();
}

wxThread::ExitCode CopyThread::Entry()
{
    wxThread::ExitCode exitcode=0;
    if (m_path_collection.size()==0)
    {
        return exitcode;
    }
    if (m_destination_directory.size()==0)
    {
        return exitcode;
    }
    if (m_extensions.size()==0 && m_contained)
    {
        return exitcode;
    }
    NormalizeDestination();


    try
    {
        bool b = CreateRelativeDir( _T(""));
        if (!b)
        {
            return (wxThread::ExitCode)-1;
        }
	for (PathCollection::const_iterator it = m_path_collection.begin();
		it!=m_path_collection.end();
		it++)
	{
		if (TestDestroy())
			break;
		m_source_directory = *it;
        tpath source_path_entry(m_source_directory.wx_str(), boost::filesystem::native);
        if (boost::filesystem::is_directory(source_path_entry)==false)
        {
            OnFile(m_source_directory);
        }
        else
        {
		    wxDir dir(m_source_directory);
		    dir.Traverse(*this);
        }
	}

    } // try
    catch(const std::exception& e)
    {
        wxString msg;
        msg.Printf(_T("CopyThread exception:%s"), e.what());
        Dump(msg);
    }
    return (wxThread::ExitCode)0;
}

wxDirTraverseResult CopyThread::OnFile( const wxString& filename )
{
    wxString msg;
    if (TestDestroy())
    {
        Dump(_T("CopyThread::OnFile cancelled"));
        return wxDIR_STOP;
    }
    tpath source_path_entry(m_source_directory.c_str(), boost::filesystem::native);
    bool source_is_file = (boost::filesystem::is_directory(source_path_entry)==false);

    // get extension
    
    size_t pos = filename.rfind(_T('.'));
    if (pos == wxString::npos && !source_is_file)
    {
        msg.Printf(_T("Skip %s"), filename.c_str());
        Dump(msg);
        return wxDIR_CONTINUE;
    }
    wxString ext = filename.substr(pos);
    ext.MakeLower();
    StringMap::iterator it = m_extensions.find(ext);
    if (  (it!=m_extensions.end() && m_contained)
         || (it==m_extensions.end() && !m_contained)
        || source_is_file)
    {
        wxString sub_name = filename.substr(m_source_directory.length());
        if (source_is_file)
        {
            sub_name = boost::filesystem::basename(source_path_entry) + boost::filesystem::extension(source_path_entry);
        }
        bool b = false;
        wxString destination_name;
        wxString msg;
        for (PathCollection::const_iterator destination_it = m_destination_directory.begin();
            destination_it != m_destination_directory.end();
            destination_it++)
        {
            destination_name = *destination_it + sub_name;
            // TODO rename to include last modify time
            if (m_add_timestamp_to_destination_filename)
            {
                tchar_string timestr = GetFileLastModify((LPCTSTR)filename);
                if (filename.find(timestr.c_str())==filename.npos)
                {
                    destination_name = *destination_it + timestr + sub_name;                    
                }
            }
            b = this->CopyModifiedFile(filename, destination_name);
            if (!b)
            {
                msg.Printf(_T("Failed to copy [%s] to [%s]"), filename.c_str(), destination_name.c_str());
                Dump(msg);
            }
        }
        if (b && it!=m_extensions.end())
        {
            it->second++;
        }
    }
    else
    {
        msg.Printf(_T("Skip %s"), filename.c_str());
        Dump(msg);
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult CopyThread::OnDir( const wxString& dirname )
{
    if (TestDestroy())
    {
        Dump(_T("CopyThread::OnDir cancelled"));
        return wxDIR_STOP;
    }
    wxString current_dir = _T("\\.");
    size_t pos = dirname.find_last_of(current_dir);
    if (pos == dirname.length()-current_dir.length())
    {
        return wxDIR_CONTINUE;
    }
    wxString parent_dir = _T("\\..");
    pos = dirname.find_last_of(parent_dir);
    if (pos==dirname.length()-parent_dir.length())
    {
        return wxDIR_CONTINUE;
    }
    wxString sub_name = dirname.substr(m_source_directory.length());
    //wxString destination_name = m_destination_directory + sub_name;
    if (!CreateRelativeDir(sub_name.wx_str()))
    {
        return wxDIR_STOP;
    }
    return wxDIR_CONTINUE;
}

CopyThread::~CopyThread()
{
    Dump(_T("CopyThread::~CopyThread"));
    wxString msg;
    for(StringMap::const_iterator it=m_extensions.begin();
    it!=m_extensions.end();
    it++)
    {
        if (it->second)
        {
            msg.Printf(_T("Copied %d %s files"), it->second, it->first.c_str());
            Dump(msg);
        }
    }
    SaveList();
}

bool CopyThread::CreateDir( const wxString& dir )
{
    if (wxDirExists(dir)==false)
    {
        std::string ansi_dir = wxConvLocal.cWX2MB(dir.c_str());
        boost::filesystem::path dir_path((const char*)ansi_dir.c_str(), boost::filesystem::native);
        bool ret = boost::filesystem::create_directory(dir_path);
        if (!ret)
        {
            wxString msg;
            msg.Printf(_T("Failed to create %s"), dir.c_str());
            wxMessageBox(msg);
            return false;
        }
    }
    return true;
}

bool CopyThread::CopyModifiedFile( const wxString source, const wxString dest )
{
    // platform dependent
    wxString msg;
    tchar_string dst_time;
    if (wxFileExists(dest))
    {
        dst_time = GetFileLastModify(dest.wx_str());
        DWORD dwAttrs = GetFileAttributes(dest);
        if (dwAttrs & FILE_ATTRIBUTE_READONLY)
        {
            dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
            BOOL ret = SetFileAttributes(dest, dwAttrs);
            if (!ret)
            {
                DWORD error = GetLastError();
                msg.Printf(_T("Failed to remove read-only attributes from %s, erorr:%08x"),
					dest.wx_str(), error);
                wxMessageBox(msg);
            }
            else
            {
                msg.Printf(_T("remove read-only attributes of %s"), dest.wx_str());
                Dump(msg);
            }
        }
    }
    tchar_string modify_time = GetFileLastModify(source.wx_str());
    FileTimeMap::iterator it_file = m_copied_files.find(source.wx_str());
    if (it_file!=m_copied_files .end()
        && it_file->second->src_time == modify_time
        && it_file->second->dst_time == dst_time
        && dst_time.length())
    {
        msg.Printf(_T("Already copied %s"), source.wx_str());
        Dump(msg);
		m_ignore_count++;
        if (m_action_type==DO_MOVE && source!=dest)
        {
            DeleteFile(source);
        }
        return true;
    }
    BOOL ret = CopyFileEx(source, dest, NULL, NULL, &m_cancel, 0);
    if (!ret)
    {
        DWORD error = GetLastError();
        msg.Printf(_T("Failed to copy file %s to %s, erorr:%08x"), source.c_str(), dest.c_str(), error);
        wxMessageBox(msg);
        return false;
    }
    else
    {
        msg.Printf(wxT("Copy %s"), dest.wx_str());
        Dump(msg);
        if (it_file == m_copied_files.end())
        {
            OFileInfoPtr file_info(new OFileInfo);
            file_info->file_name = source;
            file_info->src_time = modify_time;
            file_info->dst_time = GetFileLastModify(dest.wx_str());
            m_copied_files[source.wx_str()] = file_info;
        }
        else
        {
            it_file->second->src_time = modify_time;
            it_file->second->dst_time = GetFileLastModify(dest.wx_str());
        }
		m_copy_count++;
        if (m_action_type==DO_MOVE && source!=dest)
        {
            DeleteFile(source);
        }
    }
    return true;
}

tchar_string CopyThread::GetFileLastModify( tchar_string file_name )
{
    time_t modify_time = wxFileModificationTime(file_name);
    if (modify_time == -1)
        return _T("");
    wxDateTime dt(modify_time);
    wxString file_date_time = dt.FormatISODate() + _T(" ") + dt.FormatISOTime();
    file_date_time.Printf( wxT("%sT%02d%02d%02d"), dt.FormatISODate(), dt.GetHour(),
        dt.GetMinute(), dt.GetSecond());
    return file_date_time.wx_str();
}

void CopyThread::SaveList()
{
    wxRemoveFile(COPIED_FILE_LIST_FILE);
    if (m_copied_files.size()==0)
        return ;
    wxXmlDocument doc;
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, copy_file_root_node);
    root->AddProperty(wxT("xmlns"), wxT("http://www.fruitfruit.com/OnegaZhang/PathUtil"));
    doc.SetRoot(root);

    for (FileTimeMap::const_iterator it=m_copied_files.begin();
    it!=m_copied_files.end();
    it++)
    {
        wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, copy_file_node);
        pNode->AddProperty(new wxXmlProperty(copy_file_name, it->first));
        pNode->AddProperty(new wxXmlProperty(copy_file_time, it->second->src_time));
        pNode->AddProperty(new wxXmlProperty(copy_file_dst_time, it->second->dst_time));
        root->AddChild(pNode);
    }
    doc.Save(COPIED_FILE_LIST_FILE);

}

void CopyThread::LoadList()
{
    // Load copied file list
    if (m_copied_files.size())
        return;
    if (wxFileExists(COPIED_FILE_LIST_FILE)==false)
        return;
    wxXmlDocument doc;
    if (!doc.Load(COPIED_FILE_LIST_FILE))
        return;
    if (doc.GetRoot()->GetName() != copy_file_root_node)
        return;

    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child) {

        if (child->GetName() == copy_file_node)
        {
            // process text enclosed by <tag1></tag1>
            wxString content = child->GetNodeContent();
            // process properties of <tag1>
            wxString file_name =
                child->GetPropVal(copy_file_name,
                wxT(""));
            wxString src_time =
                child->GetPropVal(copy_file_time,
                wxT(""));
            tchar_string dst_time = child->GetPropVal(copy_file_dst_time, wxT("")).c_str();
            if ( file_name.length()
                && src_time.length()
                && dst_time.length()
                && m_copied_files.find(tchar_string(file_name))==m_copied_files.end())
            {
                OFileInfoPtr file_info(new OFileInfo);
                file_info->file_name = file_name;
                file_info->src_time = src_time;
                file_info->dst_time = dst_time;
                m_copied_files[tchar_string(file_name)] = file_info;
            }
        }
        child = child->GetNext();
    }
}

bool CopyThread::CreateRelativeDir( tchar_string relative_dir_name )
{
    bool b = false;
    for(PathCollection::const_iterator it=m_destination_directory.begin();
        it!=m_destination_directory.end();
        it++)
    {
        b = CreateDir(*it + relative_dir_name);
        if (!b)
        {
            return b;
        }
    }
    return true;
}

void CopyThread::AddTimestampToFileName( bool add )
{
    m_add_timestamp_to_destination_filename = add;
}

void CopyThread::SetActionType( FILE_ACTION_TYPE ft )
{
    m_action_type=ft;
}

void CopyThread::NormalizeDestination()
{
    bool dest_is_dir = false;
    if (m_path_collection.size()>1)
        dest_is_dir = true;
    tpath source_path_entry( m_path_collection.front(), boost::filesystem::native);
    if (boost::filesystem::is_directory(source_path_entry) )
    {
        dest_is_dir = true;
    }
    if (dest_is_dir)
    {
        //make sure they are ended with "\\"
        for (PathCollection::iterator destination_it = m_destination_directory.begin();
            destination_it != m_destination_directory.end();
            destination_it++)
        {
            if (!boost::algorithm::ends_with( *destination_it, _T("\\")))
            {
                tchar_string& tmp = *destination_it;
                tmp += _T("\\");
            }
        }
    }
}
