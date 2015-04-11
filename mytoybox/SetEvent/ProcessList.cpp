#include <windows.h>
#include <psapi.h>
#include <map>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/scoped_array.hpp>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "ntnative.h"
#pragma comment(lib,"psapi.lib")
#include "ProcessList.h"

// refer to http://www.xcodez.com/?id=112#

struct ProcessListData
{
	std::map<int, boost::shared_ptr<ProcessInfo> > m_processes;
};



#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_SUCCESS              ((NTSTATUS)0x00000000L)

ProcessList::ProcessList(void)
{
	m_pimpl = new ProcessListData;
}

ProcessList::~ProcessList(void)
{
	if (m_pimpl)
	{
		delete m_pimpl;
		m_pimpl = NULL;
	}
}

int ProcessList::GetCount()
{
	if (m_pimpl)
		return m_pimpl->m_processes.size();
	return 0;
}

typedef NTSTATUS (_stdcall *PNtQuerySystemInformation)(
	IN SYSTEM_INFORMATION_CLASS SystemProcessesAndThreadsInformation,
	OUT PVOID               SystemInformation,
	IN ULONG                SystemInformationLength,
	OUT PULONG              ReturnLength OPTIONAL);

static void EnableDebugPriv( void )
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
	{
		printf("OPT() failed, gle=%s SeDebugPrivilege is not available.",GetLastError());
		return;
	}
	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&sedebugnameValue))
	{
		printf("LPV() failed, gle=%s SeDebugPrivilege is not available.",GetLastError());
		CloseHandle( hToken );
		return;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(tkp),NULL,NULL))
		printf("ATP() failed, gle=%s SeDebugPrivilege is not available." , GetLastError());
	CloseHandle(hToken);
}

int ProcessList::Refresh()
{
	m_pimpl->m_processes.clear();
	HMODULE hNtDll;
	NTSTATUS Status;
	ULONG cbBuffer=0x8000;
	LPVOID pBuffer = NULL;
	HANDLE hProcess;
	EnableDebugPriv();
	hNtDll=LoadLibrary(_T("ntdll.dll"));
	PNtQuerySystemInformation tNtQuerySystemInformation=(PNtQuerySystemInformation)GetProcAddress(hNtDll,("NtQuerySystemInformation"));
	do{
		pBuffer=malloc(cbBuffer);
		if (pBuffer==NULL)
		{
			_tprintf(_T("Not enough memory\n"));
			return 1;
		}

		Status=tNtQuerySystemInformation(
			(SYSTEM_INFORMATION_CLASS)SystemProcessesAndThreadsInformation,
			pBuffer, cbBuffer, NULL);

		if (Status==STATUS_INFO_LENGTH_MISMATCH)
		{
			free(pBuffer);
			cbBuffer*=2;
		}
		else if (!NT_SUCCESS(Status))
		{
			_tprintf(_T("tNtQuerySystemInformation failed with")
				_T("status 0x%08X\n"), Status);

			free(pBuffer);
			return 1;
		}
	}
	while (Status==STATUS_INFO_LENGTH_MISMATCH);

	::PSYSTEM_PROCESS_INFORMATION pInfo=(::PSYSTEM_PROCESS_INFORMATION)pBuffer;

	TCHAR szFileName[MAX_PATH+1024];
	char pszProcessName[MAX_PATH];
	for (;;)
	{
		boost::shared_ptr<ProcessInfo> proc_info(new ProcessInfo);
		hProcess=NULL;
		memset(pszProcessName,0,sizeof(pszProcessName));
		szFileName[0] = 0;
		//输出进程名
		if(pInfo->ProcessName.Length==0)
		{
			proc_info->process_name = _T("Idle");
			//_tcscpy_s(pszProcessName,_T("Idle"));
		}
		else
		{
			proc_info->process_name = ToStlString(pInfo->ProcessName.Buffer);
		}
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION| PROCESS_VM_READ,FALSE,pInfo->ProcessId);
		GetModuleFileNameEx(hProcess,NULL,szFileName,MAX_PATH);
		if(hProcess!=NULL)
			CloseHandle(hProcess);
		proc_info->module_path = szFileName;
		m_pimpl->m_processes[pInfo->ProcessId] = proc_info;
		if (pInfo->NextEntryDelta==0)
			break;
		pInfo=(::PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo) + pInfo->NextEntryDelta);
	}
	free(pBuffer);
	return 0;
}


typedef NTSTATUS (__stdcall *pNtQueryInformationProcess) (
								   IN HANDLE ProcessHandle,
								   IN PROCESSINFOCLASS ProcessInformationClass,
								   OUT PVOID ProcessInformation,
								   IN ULONG ProcessInformationLength,
								   OUT PULONG ReturnLength OPTIONAL
								   );

std::basic_string<TCHAR> ProcessList::ToStlString(wchar_t* sdata, int length)
{
#if _UNICODE
	if (sdata)
		return sdata;
	return _T("");
#else
	int len = 0;
	if (length)
		len = length;
	else
		len = wcslen(sdata);
	boost::scoped_array<char> buf_holder(new char[len*2+2]);
	buf_holder[0] = 0;
	int char_count = WideCharToMultiByte(CP_ACP, 0, sdata, len, buf_holder.get(),
		len*2, 0, false);
	if (char_count>=0)
		buf_holder.get()[char_count] = 0;
	return buf_holder.get();
#endif

}
int ProcessList::GetProcessInfo( int pid, ProcessInfo& pinfo )
{
	HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION| PROCESS_VM_READ,FALSE, pid);
	if (!hprocess)
	{
		return GetLastError();
	}
	int ec = GetProcessInfo(hprocess, pinfo);
	CloseHandle(hprocess);
	return ec;
}

int ProcessList::GetProcessInfo( HANDLE hprocess, ProcessInfo& pinfo )
{
	std::basic_string<TCHAR> ret;
	TCHAR msg_buf[MAX_PATH] = {0};
	PROCESS_BASIC_INFORMATION basic_proc_info = { 0 };
	ULONG uReturnLength = 0;
	pNtQueryInformationProcess ZwQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(
		GetModuleHandle(_T("ntdll.dll")),
		"NtQueryInformationProcess");
	if (!ZwQueryInformationProcess)
	{
		return GetLastError();
	}
	NTSTATUS ntStat = ZwQueryInformationProcess( hprocess,
		ProcessBasicInformation,
		&basic_proc_info,
		sizeof(basic_proc_info),
		&uReturnLength );
	PEB remote_peb;
	DWORD bytes_read = 0;
	BOOL ecret = ReadProcessMemory(hprocess, basic_proc_info.PebBaseAddress, 
		&remote_peb, sizeof(remote_peb), &bytes_read);
	if (!ecret)
	{
		ecret = GetLastError();
		return ecret;
	}
	if (bytes_read!=sizeof(remote_peb))
	{
		return E_FAIL;
	}
	PROCESS_PARAMETERS remote_pp;
	memset(&remote_pp, 0, sizeof(remote_pp));
	ecret = ReadProcessMemory(hprocess, remote_peb.ProcessParameters, 
		&remote_pp, sizeof(remote_pp), &bytes_read);
	if (!ecret)
	{
		ecret = GetLastError();
		return ecret;
	}
	if (bytes_read!=sizeof(remote_pp))
	{
		return E_FAIL;
	}
	int buf_size = 1024+MAX_PATH;
	boost::shared_array<wchar_t> buf_holder(new wchar_t[buf_size]);
	if (!buf_holder.get())
	{
		return E_OUTOFMEMORY;
	}
	wchar_t *buf = buf_holder.get();
	memset(buf, 0, buf_size*sizeof(*buf));

	wsprintf(msg_buf, _T("length of Command line: %d, max length %d, buf_size %d"), 
		remote_pp.CommandLine.Length, remote_pp.CommandLine.MaximumLength, 
		buf_size);
	OutputDebugString(msg_buf);

	ecret = ReadProcessMemory(hprocess, remote_pp.CommandLine.Buffer, buf, 
		remote_pp.CommandLine.Length*2, &bytes_read);
	wsprintf(msg_buf, _T("length of command line: %d, read bytes: %d, max length: %d"),
		remote_pp.CommandLine.Length, bytes_read, remote_pp.CommandLine.MaximumLength);
	OutputDebugString(msg_buf);

	pinfo.command_line = ToStlString(buf);
	memset(buf, 0, buf_size*sizeof(*buf));
	ecret = ReadProcessMemory(hprocess, remote_pp.CurrentDirectory.DosPath.Buffer, buf, 
		remote_pp.CurrentDirectory.DosPath.Length, &bytes_read);
	if (!ecret)
	{
		ecret = GetLastError();
		return ecret;
	}
	pinfo.working_directory = ToStlString(buf);
	memset(buf, 0, buf_size*sizeof(*buf));

		ecret = ReadProcessMemory(hprocess, remote_pp.ImagePathName.Buffer, buf,
		remote_pp.ImagePathName.Length, &bytes_read);
		if (!ecret)
		{
			ecret = GetLastError();
			return ecret;
		}
		pinfo.module_path = ToStlString(buf);

	// get environment of the process
		MEMORY_BASIC_INFORMATION memInfo;
		int nSize = 0;
		VirtualQueryEx( hprocess, remote_pp.Environment,&memInfo,sizeof(memInfo));
		if( PAGE_NOACCESS == memInfo.Protect ||
			PAGE_EXECUTE == memInfo.Protect )
		{
			nSize = 0;
		}
		else
		{
			nSize = memInfo.RegionSize-((char*)remote_pp.Environment-(char*)memInfo.BaseAddress);
		}
		if (nSize > buf_size)
		{
			buf_size = nSize+1;
			buf_holder.reset(new wchar_t[buf_size]);
			buf = buf_holder.get();
		}
		ecret = ReadProcessMemory(hprocess, remote_pp.Environment, buf,
			nSize, &bytes_read);
		if (!ecret)
		{
			ecret = GetLastError();
			return ecret;
		}
		wsprintf(msg_buf, _T("Readable bytes: %d, read length: %d"), nSize, bytes_read);
		OutputDebugString(msg_buf);
		std::basic_stringstream<TCHAR> ts;
		while (wcslen(buf))
		{
			ts << ToStlString(buf) << std::endl;
			buf += wcslen(buf)+1;
		}
		pinfo.environment = ts.str();

	return ERROR_SUCCESS;
}