//#pragma comment(linker, "/opt:nowin98")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

#define _WIN32_WINNT	0x0501

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <shlwapi.h>
#include <tchar.h>
#include <list>
#include <string>
#include <map>

// -------------------------------------------------------------------------
typedef LONG NTSTATUS;

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef void (WINAPI * PIO_APC_ROUTINE)(PVOID, PIO_STATUS_BLOCK, DWORD);

typedef LONG TDI_STATUS;
typedef PVOID CONNECTION_CONTEXT;       // connection context

typedef struct _TDI_REQUEST {
    union {
        HANDLE AddressHandle;
        CONNECTION_CONTEXT ConnectionContext;
        HANDLE ControlChannel;
    } Handle;
	
    PVOID RequestNotifyObject;
    PVOID RequestContext;
    TDI_STATUS TdiStatus;
} TDI_REQUEST, *PTDI_REQUEST;

typedef struct _TDI_CONNECTION_INFORMATION {
    LONG UserDataLength;        // length of user data buffer
    PVOID UserData;             // pointer to user data buffer
    LONG OptionsLength;         // length of following buffer
    PVOID Options;              // pointer to buffer containing options
    LONG RemoteAddressLength;   // length of following buffer
    PVOID RemoteAddress;        // buffer containing the remote address
} TDI_CONNECTION_INFORMATION, *PTDI_CONNECTION_INFORMATION;

typedef struct _TDI_REQUEST_QUERY_INFORMATION {
    TDI_REQUEST Request;
    ULONG QueryType;                          // class of information to be queried.
    PTDI_CONNECTION_INFORMATION RequestConnectionInformation;
} TDI_REQUEST_QUERY_INFORMATION, *PTDI_REQUEST_QUERY_INFORMATION;

#define TDI_QUERY_ADDRESS_INFO			0x00000003
#define IOCTL_TDI_QUERY_INFORMATION		CTL_CODE(FILE_DEVICE_TRANSPORT, 4, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

typedef VOID *POBJECT;

typedef struct _SYSTEM_HANDLE {
	ULONG		uIdProcess;
	UCHAR		ObjectType;    // OB_TYPE_* (OB_TYPE_TYPE, etc.)
	UCHAR		Flags;         // HANDLE_FLAG_* (HANDLE_FLAG_INHERIT, etc.)
	USHORT		Handle;
	POBJECT		pObject;
	ACCESS_MASK	GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG			uCount;
	SYSTEM_HANDLE	Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef UNICODE_STRING OBJECT_NAME_INFORMATION;
typedef UNICODE_STRING *POBJECT_NAME_INFORMATION;

#define SystemHandleInformation			16

typedef enum _OBJECT_INFORMATION_CLASS{
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllTypesInformation,
	ObjectHandleInformation
} OBJECT_INFORMATION_CLASS;

#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH		((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW			((NTSTATUS)0x80000005L)
// -------------------------------------------------------------------------


typedef NTSTATUS (WINAPI *tNTQSI)(DWORD SystemInformationClass, PVOID SystemInformation,
								  DWORD SystemInformationLength, PDWORD ReturnLength);
typedef NTSTATUS (WINAPI *tNTQO)(HANDLE ObjectHandle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation,
								 DWORD Length, PDWORD ResultLength);
typedef NTSTATUS (WINAPI *tNTDIOCF)(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,
							   PIO_STATUS_BLOCK IoStatusBlock, DWORD IoControlCode,
							   PVOID InputBuffer, DWORD InputBufferLength,
							   PVOID OutputBuffer, DWORD OutputBufferLength);

void EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE) {
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE)
		{
			tokenPriv.PrivilegeCount           = 1;
			tokenPriv.Privileges[0].Luid       = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
		}
	}
}

LPWSTR GetObjectInfo(HANDLE hObject, OBJECT_INFORMATION_CLASS objInfoClass)
{
	LPWSTR lpwsReturn = NULL;
	tNTQO pNTQO = (tNTQO)GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtQueryObject");
	if(pNTQO != NULL){
		DWORD dwSize = sizeof(OBJECT_NAME_INFORMATION);
		POBJECT_NAME_INFORMATION pObjectInfo = (POBJECT_NAME_INFORMATION) new BYTE[dwSize];
		NTSTATUS ntReturn = pNTQO(hObject, objInfoClass, pObjectInfo, dwSize, &dwSize);
		if((ntReturn == STATUS_BUFFER_OVERFLOW) || (ntReturn == STATUS_INFO_LENGTH_MISMATCH)){
			delete pObjectInfo;
			pObjectInfo = (POBJECT_NAME_INFORMATION) new BYTE[dwSize];
			ntReturn = pNTQO(hObject, objInfoClass, pObjectInfo, dwSize, &dwSize);
		}
		if((ntReturn >= STATUS_SUCCESS) && (pObjectInfo->Buffer != NULL))
		{
			lpwsReturn = (LPWSTR) new BYTE[pObjectInfo->Length + sizeof(WCHAR)];
			ZeroMemory(lpwsReturn, pObjectInfo->Length + sizeof(WCHAR));
			CopyMemory(lpwsReturn, pObjectInfo->Buffer, pObjectInfo->Length);
		}
		delete pObjectInfo;
	}
	return lpwsReturn;
}

void OutputConnectionDetails(HANDLE hObject)
{
	static 	tNTDIOCF pNTDIOCF;
	if (pNTDIOCF == NULL)
		pNTDIOCF= (tNTDIOCF)GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtDeviceIoControlFile");
	if(pNTDIOCF != NULL){
		IO_STATUS_BLOCK IoStatusBlock;
		TDI_REQUEST_QUERY_INFORMATION tdiRequestAddress = {{0}, TDI_QUERY_ADDRESS_INFO};
		BYTE tdiAddress[128];
		
		HANDLE hEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);
		NTSTATUS ntReturn2 = pNTDIOCF(hObject, hEvent2, NULL, NULL, &IoStatusBlock, IOCTL_TDI_QUERY_INFORMATION,
			&tdiRequestAddress, sizeof(tdiRequestAddress), &tdiAddress, sizeof(tdiAddress));
		if(hEvent2) CloseHandle(hEvent2);

		if(ntReturn2 == STATUS_SUCCESS){
			struct in_addr *pAddr = (struct in_addr *)&tdiAddress[14];
			printf("@%s:%d", inet_ntoa(*pAddr), ntohs(*(PUSHORT)&tdiAddress[12]));
		}
	}
	printf("\n");
}

namespace Napalm
{
	int main(int argc, char *argv[])
	{
		printf("TCP/UDP Handle List - by Napalm\n");
		printf("===============================\n\n");

		EnableDebugPrivilege();

		tNTQSI pNTQSI = (tNTQSI)GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtQuerySystemInformation");
		if(pNTQSI != NULL){
			DWORD dwSize = sizeof(SYSTEM_HANDLE_INFORMATION);
			PSYSTEM_HANDLE_INFORMATION pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
			NTSTATUS ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
			if(ntReturn == STATUS_INFO_LENGTH_MISMATCH){
				delete pHandleInfo;
				pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
				ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
			}
			if(ntReturn == STATUS_SUCCESS){
				printf(" Found %d Handles.\n\n", pHandleInfo->uCount);
				printf("  PID\tHandle\t%-16s%-18sHandle Name\n", "Type", "Process Name");
				for(DWORD dwIdx = 0; dwIdx < pHandleInfo->uCount; dwIdx++)
				{
					HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
						FALSE, pHandleInfo->Handles[dwIdx].uIdProcess);
					if(hProcess != INVALID_HANDLE_VALUE)
					{
						HANDLE hObject = NULL;
						if(DuplicateHandle(hProcess, (HANDLE)pHandleInfo->Handles[dwIdx].Handle,
							GetCurrentProcess(), &hObject, STANDARD_RIGHTS_REQUIRED, FALSE, 0) != FALSE)
						{
							//LPWSTR lpwsName = GetObjectInfo(hObject, ObjectNameInformation);
							//if(lpwsName != NULL){
							//	LPWSTR lpwsType = GetObjectInfo(hObject, ObjectTypeInformation);
							//	LPSTR lpszProcess = new CHAR[MAX_PATH];
							//	ZeroMemory(lpszProcess, MAX_PATH);
							//	GetModuleFileNameEx(hProcess, NULL, lpszProcess, MAX_PATH);
							//	printf("%5d\t%6d\t%-16ws%-18s%ws",
							//		pHandleInfo->Handles[dwIdx].uIdProcess, pHandleInfo->Handles[dwIdx].Handle, lpwsType,
							//		((lstrlen(lpszProcess) > 0)?PathFindFileName(lpszProcess):"[System]"), lpwsName);
							//	if(!wcscmp(lpwsName, L"\\Device\\Tcp") || !wcscmp(lpwsName, L"\\Device\\Udp"))
							//		OutputConnectionDetails(hObject);
							//	else
							//		printf("\n");
							//	delete lpwsName, lpwsType, lpszProcess;
							//}
							CloseHandle(hObject);
						}
						CloseHandle(hProcess);
					}	
				}
				printf("\n\n");
			}else{
				printf("Error while trying to allocate memory for System Handle Information.\n");
			}
			delete pHandleInfo;
		}else{
			printf("Cannot find NtQuerySystemInformation API... Is this system not Win2K and above?");
		}
		
		return 0;
	}
}



int get_event_list(DWORD process_id, std::list<std::wstring>& event_list)
{
	EnableDebugPrivilege();

	tNTQSI pNTQSI = (tNTQSI)GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtQuerySystemInformation");
	if(pNTQSI != NULL){
		DWORD dwSize = sizeof(SYSTEM_HANDLE_INFORMATION);
		PSYSTEM_HANDLE_INFORMATION pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
		NTSTATUS ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		if(ntReturn == STATUS_INFO_LENGTH_MISMATCH){
			delete pHandleInfo;
			pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
			ntReturn = pNTQSI(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		}
		if(ntReturn == STATUS_SUCCESS){
			printf(" Found %d Handles.\n\n", pHandleInfo->uCount);
			printf("  PID\tHandle\t%-16s%-18sHandle Name\n", "Type", "Process Name");
			for(DWORD dwIdx = 0; dwIdx < pHandleInfo->uCount; dwIdx++)
			{
				if (process_id != pHandleInfo->Handles[dwIdx].uIdProcess)
					continue;
				HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
					FALSE, pHandleInfo->Handles[dwIdx].uIdProcess);
				if(hProcess != INVALID_HANDLE_VALUE)
				{
					HANDLE hObject = NULL;
					if(DuplicateHandle(hProcess, (HANDLE)pHandleInfo->Handles[dwIdx].Handle,
						GetCurrentProcess(), &hObject, STANDARD_RIGHTS_REQUIRED, FALSE, 0) != FALSE)
					{
						LPWSTR lpwsName = GetObjectInfo(hObject, ObjectNameInformation);
						if(lpwsName != NULL){
							LPWSTR lpwsType = GetObjectInfo(hObject, ObjectTypeInformation);
							if (wcscmp(lpwsType, L"Event")==0)
							{
								event_list.push_back(lpwsName);
							}
							LPSTR lpszProcess = new CHAR[MAX_PATH];
							//ZeroMemory(lpszProcess, MAX_PATH);
							//GetModuleFileNameEx(hProcess, NULL, lpszProcess, MAX_PATH);
							//printf("%5d\t%6d\t%-16ws%-18s%ws",
							//	pHandleInfo->Handles[dwIdx].uIdProcess, pHandleInfo->Handles[dwIdx].Handle, lpwsType,
							//	((lstrlen(lpszProcess) > 0)?PathFindFileName(lpszProcess):"[System]"), lpwsName);
							//if(!wcscmp(lpwsName, L"\\Device\\Tcp") || !wcscmp(lpwsName, L"\\Device\\Udp"))
							//	OutputConnectionDetails(hObject);
							//else
							//	printf("\n");
							delete lpwsName, lpwsType, lpszProcess;
						}
						CloseHandle(hObject);
					}
					CloseHandle(hProcess);
				}	
			}
			printf("\n\n");
		}else{
			printf("Error while trying to allocate memory for System Handle Information.\n");
		}
		delete pHandleInfo;
	}else{
		printf("Cannot find NtQuerySystemInformation API... Is this system not Win2K and above?");
	}

	return 0;
}

int get_handle_list(DWORD process_id, std::list<std::wstring>& event_list)
{
	EnableDebugPrivilege();
	std::map<std::wstring, std::wstring> allhandles;

	tNTQSI ZwQuerySystemInformation = (tNTQSI)GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtQuerySystemInformation");
	if(ZwQuerySystemInformation != NULL){
		DWORD dwSize = sizeof(SYSTEM_HANDLE_INFORMATION);
		PSYSTEM_HANDLE_INFORMATION pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
		NTSTATUS ntReturn = ZwQuerySystemInformation(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		if(ntReturn == STATUS_INFO_LENGTH_MISMATCH){
			delete pHandleInfo;
			pHandleInfo = (PSYSTEM_HANDLE_INFORMATION) new BYTE[dwSize];
			ntReturn = ZwQuerySystemInformation(SystemHandleInformation, pHandleInfo, dwSize, &dwSize);
		}
		if(ntReturn == STATUS_SUCCESS){
			printf(" Found %d Handles.\n\n", pHandleInfo->uCount);
			printf("  PID\tHandle\t%-16s%-18sHandle Name\n", "Type", "Process Name");
			for(DWORD dwIdx = 0; dwIdx < pHandleInfo->uCount; dwIdx++)
			{
				if (process_id != pHandleInfo->Handles[dwIdx].uIdProcess)
					continue;
				HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
					FALSE, pHandleInfo->Handles[dwIdx].uIdProcess);
				if(hProcess != INVALID_HANDLE_VALUE)
				{
					HANDLE hObject = NULL;
					if(DuplicateHandle(hProcess, (HANDLE)pHandleInfo->Handles[dwIdx].Handle,
						GetCurrentProcess(), &hObject, STANDARD_RIGHTS_REQUIRED, FALSE, 0) != FALSE)
					{
						LPWSTR lpwsName = GetObjectInfo(hObject, ObjectNameInformation);
						if(lpwsName != NULL){
							LPWSTR lpwsType = GetObjectInfo(hObject, ObjectTypeInformation);
							wchar_t buf[64];
							wsprintfW(buf, L"%s %04x", lpwsType, pHandleInfo->Handles[dwIdx].Handle);
							allhandles[buf] = lpwsName;
							//if(!wcscmp(lpwsName, L"\\Device\\Tcp") || !wcscmp(lpwsName, L"\\Device\\Udp"))
							//	OutputConnectionDetails(hObject);
							//else
							//	printf("\n");
							delete lpwsName, lpwsType;
						}
						CloseHandle(hObject);
					}
					CloseHandle(hProcess);
				}	
			}
			printf("\n\n");
		}else{
			printf("Error while trying to allocate memory for System Handle Information.\n");
		}
		delete pHandleInfo;
	}else{
		printf("Cannot find NtQuerySystemInformation API... Is this system not Win2K and above?");
	}
	for(std::map<std::wstring, std::wstring>::const_iterator it = allhandles.begin();
		it!=allhandles.end();
		it++)
	{
		std::wstring wret = it->first + L" " + it->second;
		event_list.push_back(wret);
	}
	return 0;
}
