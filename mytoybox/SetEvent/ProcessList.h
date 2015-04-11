#pragma once
#include <string>

struct ProcessInfo
{
	std::basic_string<TCHAR> process_name;
	std::basic_string<TCHAR> module_path;
	std::basic_string<TCHAR> working_directory;
	std::basic_string<TCHAR> command_line;
	std::basic_string<TCHAR> environment;
};

class ProcessList
{
public:
	ProcessList(void);
	~ProcessList(void);
	int GetCount();
	int Refresh();

	//************************************
	// Method:    GetProcessInfo
	// Brief:     return ERROR_SUCCESS if no error, otherwise return Win32 error code
	// Returns:   int
	// Parameter: int pid
	// Parameter: ProcessInfo & pinfo
	//************************************
	static int GetProcessInfo(int pid, ProcessInfo& pinfo);
	static std::basic_string<TCHAR> ToStlString(wchar_t* sdata, int length=0);
	static int GetProcessInfo(HANDLE hprocess, ProcessInfo& pinfo);
private:
	struct ProcessListData* m_pimpl;
};
