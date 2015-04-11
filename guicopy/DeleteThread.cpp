#include "guicopy_include.h"
#include "mydlg.h"
#include "DeleteThread.h"

DeleteThread::DeleteThread( const wxString source,
						   const wxArrayString& extension_list )
						   : OBaseThread( extension_list)
{
    m_file_count=m_dir_count=0;
    Parse(source, m_path_collection);
}

DeleteThread::~DeleteThread()
{
    wxString msg;
    msg.Printf(_T("DeleteThread::~DeleteThread processed %d files, %d folders"), m_file_count, m_dir_count);
    Dump(msg);
    for(StringMap::const_iterator it=m_extensions.begin();
    it!=m_extensions.end();
    it++)
    {
        msg.Printf(_T("Deleted %d %s files"), it->second, it->first.wx_str());
        Dump(msg);
    }
}

wxThread::ExitCode DeleteThread::Entry()
{
    wxThread::ExitCode exitcode=0;
    if (m_extensions.size()==0)
    {
        return exitcode;
    }
    try
    {
		for (PathCollection::const_iterator it = m_path_collection.begin();
			it!=m_path_collection.end();
			it++)
		{
            Dump(*it);
			if (TestDestroy())
				break;
			wxDir dir(*it);
			dir.Traverse(*this);
		}

    } // try
    catch(int i)
    {
        i = i;
    }
    return (wxThread::ExitCode)0;
}

wxDirTraverseResult DeleteThread::OnFile( const wxString& filename )
{
    wxString msg;
    if (TestDestroy())
    {
        Dump(_T("DeleteThread cancelled"));
        return wxDIR_STOP;
    }
    m_file_count++;
    // get extension    
    size_t pos = filename.rfind(_T('.'));
    if (pos==wxString::npos)
    {
        msg.Printf(_T("Skip %s"), (LPCTSTR)filename);
        return (wxDirTraverseResult)(wxDIR_CONTINUE);
    }
    wxString ext = filename.substr(pos);
    ext.MakeLower();
    std::map<wxString,int>::iterator it = m_extensions.find(ext);
    if (it!=m_extensions.end())
    {
        bool ret = wxRemoveFile(filename);
        if (!ret)
        {
            msg.Printf(_T("Failed to delete %s"), (LPCTSTR)filename);
        }
        else
        {
            it->second += 1;
            msg.Printf(_T("Delete %s"), (LPCTSTR)filename);
        }
    }
    else
    {
        msg.Printf(_T("Skip %s"), (LPCTSTR)filename);
        if (filename.find("deploy.bat")!=tchar_string::npos)
        {
            msg.Printf(_T("Skip %s"), (LPCTSTR)filename);
        }
    }
    if (msg.length())
    {
        Dump(msg.wx_str());
    }
    return (wxDirTraverseResult)(wxDIR_CONTINUE);
}

wxDirTraverseResult DeleteThread::OnDir( const wxString& dirname )
{
    if (TestDestroy())
    {
        Dump(_T("DeleteThread cancelled"));
        return wxDIR_STOP;
    }
    wxString msg;
    msg = __FUNCTION__ " ";
    msg += dirname;
    Dump(msg);
    m_dir_count++;
    return wxDIR_CONTINUE;
}
