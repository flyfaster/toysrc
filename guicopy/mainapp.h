#pragma once 
#include "guicopy_types.h"
class PathUtilApp: public wxApp
{
public:
	PathUtilApp();
	~PathUtilApp();
	bool OnInit();

	void CreateShm();

	//************************************
	// Method:    ParseOptions
	// Brief:     Parse arguments in command line to this executable
	// Returns:   bool
	//************************************
	bool ParseOptions();

    void AttachToConsole();

    int OnExit();
	static void Dump(const tchar_string msg);
	void HandleEvent(wxEvtHandler *handler,
		wxEventFunction func,
		wxEvent& event) const;
	static LPCTSTR GetDefaultConfigFileName();
	static bool IsDirExists(wxString dirname);
	void Close();
	static void SetSizer(wxWindow* wnd, wxSizer* sizer);

	//************************************
	// Method:    GetDirSize
	// Brief:     Return size of all files under a directory, including its children directory
	// Returns:   long long
	// Parameter: const std::string sdir_path
	//************************************
	static long long GetDirSize(const tchar_string sdir_path);

	tchar_string ModuleName;
	tchar_string ModuleDirectory;

	//************************************
	// Method:    GetModulePath
	// Brief:     Return the folder name where the executable is located
	// Returns:   std::string
	//************************************
	tchar_string GetModulePath();

	//************************************
	// Method:    GetMemoryString
	// Brief:     Return content of specified shared memory as string
	// Returns:   std::string
	// Parameter: const std::string & mem_name
	//************************************
    tchar_string GetMemoryString(const tchar_string& mem_name);

	//************************************
	// Method:    UpdateMemoryString
	// Brief:     Copy passed content to specified shared memory region
	// Returns:   bool
	// Parameter: const std::string & mem_name
	// Parameter: const std::string & content
	//************************************
    bool UpdateMemoryString(const tchar_string& mem_name, const tchar_string& content);

	//************************************
	// Method:    ExportProperty
	// Brief:     Register property and its storage, so that when the property is changed
	//            by external tool, a workthread can update its value to value storage 
	//            automatically.
	// Returns:   void
	// Parameter: const std::string & name
	// Parameter: int & value_storage
	//************************************
    void ExportProperty(const tchar_string& name, int& value_storage);
	
	//************************************
	// Method:    RefreshProperties
	// FullName:  PathUtilApp::RefreshProperties
	// Brief:     Read shared memory and parse all properties, store their values to 
	//            registered property storage.
	// Returns:   void
	// Qualifier:
	//************************************
	void RefreshProperties();

    void PathUtilApp::copy_changed_file(boost::filesystem::path src, boost::filesystem::path dst_path);

private:
	tchar_string ModulePath;
	boost::scoped_ptr<boost::interprocess::windows_shared_memory> m_shm;
	typedef std::map<tchar_string, int*> PropertySet;
	//PropertySet m_properties;
	void OnKeyDown(wxKeyEvent& event);
	BOOL m_attach_console;
	DECLARE_EVENT_TABLE()
};

tchar_string to_tchar_string(const std::string& src);
tchar_string to_tchar_string(const std::wstring& src);
std::string to_string(const tchar_string& src);

DECLARE_APP(PathUtilApp);