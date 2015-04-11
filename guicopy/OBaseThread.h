#pragma once 
class OBaseThread: public wxThread
{
public:
	OBaseThread( const wxArrayString& extension_list);
    virtual ~OBaseThread();
    BOOL m_cancel;
private:
    wxDateTime m_start_time;
protected:
    StringMap m_extensions;
    wxString m_source_directory;
    void Dump(const wxString& msg);
    static tchar_string NormalizeDirName(tchar_string s);
    typedef std::list<tchar_string> PathCollection;
    PathCollection m_path_collection;
    static void Parse(wxString s,PathCollection& m_path_collection);
};

