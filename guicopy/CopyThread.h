#pragma once 
#include "OBaseThread.h"

class CopyThread : public OBaseThread, public wxDirTraverser
{
public:
    CopyThread(const wxString source, 
        const wxString destination, const wxArrayString& extension_list,
        bool contained);
    ~CopyThread();
    
    virtual ExitCode Entry(); 

    void NormalizeDestination();

    wxThreadError Delete(ExitCode *rc = (ExitCode *)NULL);
    void AddTimestampToFileName(bool add);
    enum FILE_ACTION_TYPE{
        DO_COPY, DO_MOVE, DO_RENAME,
    };
    void SetActionType(FILE_ACTION_TYPE ft);
private:
    PathCollection m_destination_directory;
    bool m_contained;
	int m_copy_count; // actually copied
	int m_skip_count; // 
	int m_ignore_count;
    bool m_add_timestamp_to_destination_filename;
    FILE_ACTION_TYPE m_action_type;
    virtual wxDirTraverseResult OnFile(const wxString& filename);
    
    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));   
    
    bool CreateDir(const wxString& dir);
    
    bool CopyModifiedFile(const wxString source, const wxString dest);

    // typedef std::map<tchar_string, tchar_string> FileTimeMap;
    struct OFileInfo
    {
        tchar_string file_name;
        tchar_string src_time;
        tchar_string dst_time;
    };
    typedef boost::shared_ptr<OFileInfo> OFileInfoPtr;    
    typedef std::map<tchar_string, OFileInfoPtr> FileTimeMap;
    static FileTimeMap m_copied_files;

    tchar_string GetFileLastModify(tchar_string file_name);
    void SaveList();
    void LoadList();
    //PathCollection m_destination_collection;
    bool CreateRelativeDir(tchar_string relative_dir_name);
}; 

