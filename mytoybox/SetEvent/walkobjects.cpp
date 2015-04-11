//WalkObject function debug application version, 
//must work, generates output_debug.txt in current directory with 
//listed system objects for all processes

//TURN OFF BUFFER SECURITY CHECK TO COMPILE THIS

//a part of NT Walker Library project, (c) 2004-2006 EP_X
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "ntnative.h"
#include "walkobjects.h"

//#pragma comment(linker,"/ENTRY:main")
//#pragma comment(linker,"/NODEFAULTLIB")

ULONG cbBuffer = 0x8000;//default allocation memory block size
LPVOID pBuffer = NULL;
BOOL IsServer2k03 = FALSE;
HINSTANCE hNtDll = 0;

__declspec(naked) char * __stdcall uitoa(unsigned long x, char *buf)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push edi
		sub esp, 16
		mov edi, esp
		mov eax, dword ptr x
		xor ecx, ecx
		mov cl, 10
	m1:
		xor edx, edx
		div ecx
		or dl, '0'
		mov byte ptr [edi], dl
		inc edi
		test eax, eax
		jnz m1
		mov edx, dword ptr buf
	m2:
		dec edi
		mov cl, byte ptr [edi]
		mov byte ptr [edx], cl
		inc edx
		cmp esp, edi
		jnz m2
		mov byte ptr [edx], 0
		add esp, 16
		pop edi
		mov eax, dword ptr buf
		pop ebp
		retn 0x0008
	}
}

__declspec(naked) char * __fastcall strcpyA(char *dest, const char *src)
{
	__asm
	{
		push edi
		push esi
		mov esi, ecx
		mov edi, edx
		xor ecx, ecx
		dec ecx
		xor eax, eax
		repnz scasb
		not ecx
		mov edi, esi
		mov esi, edx
		mov eax, edi
		mov edx, ecx
		shr ecx, 2
		repnz movsd
		mov ecx, edx
		and ecx, 3
		repnz movsb
		pop esi
		pop edi
		retn
	}
}

__declspec(naked) int __fastcall strlenA(char *src)
{
	__asm
	{
		mov edx, edi
		mov edi, ecx
		xor ecx, ecx
		dec ecx
		xor eax, eax
		repne scasb
		dec eax
		dec eax
		sub eax, ecx
		mov edi, edx
		retn
	}
}

//Get Nt* functions addresses

void AllocFuncAddresses()
{
	hNtDll = GetModuleHandle(_T("ntdll.dll"));
	*(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(hNtDll, "RtlAdjustPrivilege");
	*(FARPROC *)&NtQueryObject = GetProcAddress(hNtDll, "NtQueryObject");
	*(FARPROC *)&NtQuerySystemInformation = GetProcAddress(hNtDll, "NtQuerySystemInformation");
	*(FARPROC *)&NtQueryInformationFile = GetProcAddress(hNtDll, "NtQueryInformationFile");
}

//NtQuerySystemInformation with enums
INT NTAPI InitializeZwSubSystem(
	IN SYSTEM_INFORMATION_CLASS enFlag, 
	IN DWORD BytesRead
	)
{
	if (NtQuerySystemInformation == NULL) return 16;
	cbBuffer = 0x8000;
	NTSTATUS Status = 0;
	do
	{	
		pBuffer = LocalAlloc(LPTR, cbBuffer); 
		if (pBuffer == NULL)
		{
			return 8;
		}
		Status = NtQuerySystemInformation(
					enFlag,
					pBuffer, cbBuffer, &BytesRead);
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			LocalFree(pBuffer);
			cbBuffer *= 2;
		}
		else if (!NT_SUCCESS(Status))
		{
			LocalFree(pBuffer);
			return 16;
		}
	}
	while (Status == STATUS_INFO_LENGTH_MISMATCH);
	return 0;
}

//obtain system handles
INT NTAPI WalkHandles(
	OUT PHandlesEntry dwHdArray
	)
{
    ULONG i, n, *q;
    PSYSTEM_HANDLE_INFORMATION p = NULL;
	n = InitializeZwSubSystem(SystemHandleInformation, NULL);
	if (n != 0) return -1;
	q = (PULONG)pBuffer;
	if (q == NULL) return -1;
	p = (PSYSTEM_HANDLE_INFORMATION)(q + 1);
    dwHdArray->Handles = (PWALKER_HANDLE)LocalAlloc(LPTR, (ULONG)*q * sizeof(WALKER_HANDLE)); 
	for (i = 0; i < *q; i++)
    {
		dwHdArray->Handles[i].ProcessId = p[i].ProcessId;
		dwHdArray->Handles[i].Flags = p[i].Flags;
		dwHdArray->Handles[i].GrantedAccess = p[i].GrantedAccess;
		dwHdArray->Handles[i].Handle = p[i].Handle;
		dwHdArray->Handles[i].Object = p[i].Object;
		dwHdArray->Handles[i].ObjectTypeNumber = p[i].ObjectTypeNumber;
	}
	dwHdArray->HandlesCount = i;
	LocalFree(q);
	return 0;
}

DWORD NTAPI FilenameFromHandle(HTT *htt)
{
	BYTE buff[1024];
	for (int i = 0; i < 1024; i++) buff[i] = 0;
	DWORD retval;
	IO_STATUS_BLOCK iob;
	
	retval = NtQueryInformationFile(htt->handle, &iob, buff, 1024, FileNameInformation);
	if (!retval)
	{
		WCHAR *ustring = (WCHAR*)&buff[4];
		WideCharToMultiByte(CP_ACP, 0, ustring, -1, htt->txt,
				1024, NULL, NULL);
	}
	return retval;
}


DWORD QueryObjectName(HANDLE handle, char *txt)
{
	PDWORD buff = (PDWORD)LocalAlloc(LPTR, 0x1000);
	DWORD retval = 0;
	DWORD num_bytes = 0;
	txt[0] = (char)0;
	NtQueryObject(handle, ObjectNameInformation, buff, 0, &num_bytes);
	if (num_bytes && !IsServer2k03)
	{
		retval = NtQueryObject(handle, ObjectNameInformation, buff, num_bytes, NULL);	
		if (!retval && buff[1])
			WideCharToMultiByte(CP_ACP, 0, (WCHAR*)buff[1], -1, txt,
				1024, NULL, NULL);
	}
	else
	{
		DWORD tid;
		HANDLE hthread;
		HTT htt = {handle, txt};
		hthread = CreateThread(NULL, 0,	(LPTHREAD_START_ROUTINE)FilenameFromHandle, &htt, 0, &tid);
		if (WaitForSingleObjectEx(hthread, 500, FALSE) == WAIT_TIMEOUT)
		{	
			TerminateThread(hthread, 0);
			strcpyA(txt, "THREAD BLOCKED... ACCESS DENIED!");
			retval = 1;
		}
		else
		{
			LPSTR tmp = (LPSTR)LocalAlloc(LPTR, 1024);
			retval = NtQueryObject(handle, ObjectNameInformation, buff, 0x1000, NULL);
			if (!retval && buff[1])
			{
				WideCharToMultiByte(CP_ACP, 0, (WCHAR*)buff[1], -1, txt,
				1024, NULL, NULL);
				if (strlenA(tmp) > strlenA(txt))
					strcpyA(txt, tmp);
			}
			LocalFree(tmp);
		}
		CloseHandle (hthread);
	}
	LocalFree(buff);
	return retval;
}


//this is proto debug version of walkobjects function

INT NTAPI WalkObjects(
	OUT PObjectsEntry dwObjArray
	)
{
	PHandlesEntry ph1 = (PHandlesEntry)LocalAlloc(LPTR, sizeof HandlesEntry);
	if (ph1 == NULL) return -3; //mem alloc error 
	if (WalkHandles(ph1) != 0)
	{	
		LocalFree(ph1);
		return -4;
	}
	HANDLE hObject = NULL;
	HANDLE hProcess = NULL;
	ULONG n = 0;
	
	char textbuf[MAX_PATH]; //Buffer security check should be disabled to compile this
	
	HANDLE h = CreateFile(_T("output_debug.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	dwObjArray->ObjectsCount = ph1->HandlesCount;
	DWORD bytesIO = 0;
	dwObjArray->Objects = (PWALKER_OBJECT_INFORMATION)
		LocalAlloc(LPTR, dwObjArray->ObjectsCount * sizeof(WALKER_OBJECT_INFORMATION));
	for (ULONG i = 0; i < ph1->HandlesCount; i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, TRUE, ph1->Handles[i].ProcessId);
		if (hProcess == NULL) continue;
		if (DuplicateHandle(hProcess, HANDLE(ph1->Handles[i].Handle), GetCurrentProcess(), 
			&hObject, 0, TRUE, DUPLICATE_SAME_ACCESS)) 
		{
			NtQueryObject(hObject, ObjectBasicInformation,	&dwObjArray->Objects[i].obi, sizeof dwObjArray->Objects[i].obi, &n);
			n = dwObjArray->Objects[i].obi.TypeInformationLength + 2;
			dwObjArray->Objects[i].oti = (POBJECT_TYPE_INFORMATION)LocalAlloc(LPTR, n);
			NtQueryObject(hObject, ObjectTypeInformation, dwObjArray->Objects[i].oti, n, &n);			
			QueryObjectName(hObject, dwObjArray->Objects[i].oni);
			CloseHandle(hObject);

			if ((dwObjArray->Objects[i].oti == NULL) || (dwObjArray->Objects[i].oni == NULL)) continue;
		
			//output debug info
			textbuf[0] = 10;
			textbuf[1] = 0;
			WriteFile(h, textbuf, strlenA(textbuf), &bytesIO, NULL);

			strcpyA(textbuf, "Process ID: ");
			uitoa(ph1->Handles[i].ProcessId, &textbuf[12]);
			n = strlenA(textbuf);
			textbuf[n] = 10;
			textbuf[n + 1] = 0;
			WriteFile(h, textbuf, strlenA(textbuf), &bytesIO, NULL);

			WideCharToMultiByte(CP_ACP, 0, dwObjArray->Objects[i].oti->Name.Buffer, -1, 
				textbuf, MAX_PATH, NULL, NULL);
			n = strlenA(textbuf);
			textbuf[n] = 10;
			textbuf[n + 1] = 0;

			WriteFile(h, textbuf, strlenA(textbuf), &bytesIO, NULL);
			n = strlenA(dwObjArray->Objects[i].oni);
			dwObjArray->Objects[i].oni[n] = 10;
			dwObjArray->Objects[i].oni[n + 1] = 0;
			WriteFile(h, dwObjArray->Objects[i].oni, strlenA(dwObjArray->Objects[i].oni), &bytesIO, NULL);
			strcpyA(textbuf, "---------------------------------------------------");
			WriteFile(h, textbuf, strlenA(textbuf), &bytesIO, NULL);
		}
		CloseHandle(hProcess);
	}
	CloseHandle(h);
    LocalFree(ph1->Handles); 
	LocalFree(ph1);
	return 0;
}

namespace EP_X0FF{

int main()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx(&osvi);
	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) return -2;
	if (osvi.dwMajorVersion < 4) 
	{
		return -2;
	}
    IsServer2k03 = (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2);

	AllocFuncAddresses();
	BOOLEAN bprev = FALSE;
	RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, AdjustCurrentProcess, &bprev);
	PObjectsEntry ObjArray = (PObjectsEntry)LocalAlloc(LPTR, sizeof(ObjectsEntry));
	WalkObjects(ObjArray);
	LocalFree(ObjArray->Objects);
	LocalFree(ObjArray);
	return 0==0;
}

}
