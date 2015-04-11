#pragma once 
#include "OBaseThread.h"

class DeleteThread : public OBaseThread, public wxDirTraverser
{
public:
    DeleteThread(const wxString source, 
        const wxArrayString& extension_list);
    ~DeleteThread();
    
    virtual ExitCode Entry(); 
private:
    
    virtual wxDirTraverseResult OnFile(const wxString& filename);
    
    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)); 
    int m_file_count;
    int m_dir_count;
}; 
