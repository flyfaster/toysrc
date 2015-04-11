#include "guicopy_include.h"
#include "guicopy_types.h"
#include "mainapp.h"
#include "mydlg.h"
#include "OBaseThread.h"

OBaseThread::OBaseThread( const wxArrayString& extension_list )
: m_cancel(FALSE)
{
    m_start_time = wxDateTime::Now();
	SingleThreadsCol::Instance().AddThread(this);
    for (size_t i=0; i< extension_list.Count(); i++)
    {
        wxString s = extension_list.Item(i);
        s.MakeLower();
        m_extensions[s] = 0;
    }
    Create();
}

OBaseThread::~OBaseThread()
{
    wxTimeSpan duration = wxDateTime::Now() - m_start_time;
    wxString msg;
    if (duration.GetSeconds()>4)
    {
        msg.Printf(_T("Thread spend %d seconds"), duration.GetSeconds().ToLong());
    }
    else
    {
        msg.Printf(_T("Threads spend %d milliseconds"), duration.GetMilliseconds().ToLong());
    }
    Dump(msg);

	SingleThreadsCol::Instance().RemoveThread(this);
}

void OBaseThread::Dump( const wxString& msg )
{
	SingleThreadsCol::Instance().Dump(msg.wx_str());
//     wxLogVerbose(msg);
}

void OBaseThread::Parse( wxString s,PathCollection& m_path_collection )
{
    boost::char_separator<TCHAR> sep( _T("\n"));
    tchar_string str = s.c_str();
    tokenizer tokens(str, sep);
    tokenizer::iterator tok_iter = tokens.begin();
    tchar_string one_line;
    while(tok_iter!=tokens.end())
    {
        one_line = *tok_iter++;
        if (one_line.length())
            m_path_collection.push_back(NormalizeDirName(one_line));
    }

}

tchar_string OBaseThread::NormalizeDirName( tchar_string path_string )
{
    tchar_string ret = path_string;
#ifdef _UNICODE
	typedef boost::filesystem::wpath tpath;
#else
	typedef boost::filesystem::path tpath;
#endif
    tpath source_path_entry(path_string, boost::filesystem::native);

    if (path_string.length()
        && path_string.find_last_of(_T('\\'))!=path_string.length()-1
        && boost::filesystem::is_directory(source_path_entry))
    {
        ret = path_string + _T("\\");
    }
    return ret;
}
