#pragma once
#ifndef _SELF_VERIFY_HPP
#define _SELF_VERIFY_HPP

BOOL SelfDelete() 
{ 
	SHELLEXECUTEINFO sei; 
	TCHAR szModule [MAX_PATH], 
		szComspec[MAX_PATH], 
		szParams [MAX_PATH]; // get file path names: 
	if((GetModuleFileName(0,szModule,MAX_PATH)!=0) && 
		(GetShortPathName(szModule,szModule,MAX_PATH)!=0) && 
		(GetEnvironmentVariable("COMSPEC",szComspec,MAX_PATH)!=0)) 
	{ 
		// set command shell parameters 
		lstrcpy(szParams,"/c del "); 
		lstrcat(szParams, szModule); 
		lstrcat(szParams, " > nul"); // set struct members 
		sei.cbSize = sizeof(sei); 
		sei.hwnd = 0; 
		sei.lpVerb = "Open"; 
		sei.lpFile = szComspec; 
		sei.lpParameters = szParams; 
		sei.lpDirectory = 0; 
		sei.nShow = SW_HIDE; 
		sei.fMask = SEE_MASK_NOCLOSEPROCESS; // increase resource allocation to program 
		SetPriorityClass(GetCurrentProcess(), 
			REALTIME_PRIORITY_CLASS); 
		SetThreadPriority(GetCurrentThread(), 
			THREAD_PRIORITY_TIME_CRITICAL); 
		// invoke command shell 
		if(ShellExecuteEx(&sei)) 
		{ 
			// suppress command shell process until program exits 
			SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS); 
			SetProcessPriorityBoost(sei.hProcess,TRUE); 
			// notify explorer shell of deletion 
			SHChangeNotify(SHCNE_DELETE,SHCNF_PATH,szModule,0); 
			return TRUE; 
		} 
		else // if error, normalize allocation 
		{ 
			SetPriorityClass(GetCurrentProcess(), 
				NORMAL_PRIORITY_CLASS); 
			SetThreadPriority(GetCurrentThread(), 
				THREAD_PRIORITY_NORMAL); 
		} 
	} 
	return FALSE; 
} 
bool copy_file(LPCTSTR src,LPCTSTR dest)
{
	HANDLE hReadFile = NULL;
	HANDLE hWriteFile = NULL;
	bool operation_ok = false;
	hReadFile = CreateFile(src,          
		GENERIC_READ,              // open for reading 
		FILE_SHARE_READ,           // share for reading 
		NULL,                      // no security 
		OPEN_EXISTING,             // existing file only 
		FILE_ATTRIBUTE_NORMAL,     // normal file 
		NULL);                     // no attr.  
	if (hReadFile == INVALID_HANDLE_VALUE) 
	{ 
		goto clean_rename_file;
	}
	hWriteFile = CreateFile(dest,          
		GENERIC_WRITE,              // open for reading 
		FILE_SHARE_READ,           // share for reading 
		NULL,                      // no security 
		CREATE_ALWAYS,             // existing file only 
		FILE_ATTRIBUTE_NORMAL,     // normal file 
		NULL);                     // no attr.  
	if (hWriteFile == INVALID_HANDLE_VALUE) 
	{ 
		goto clean_rename_file;
	}
	char buf[4096];
	DWORD dwread = 0;
	do {
		ReadFile(hReadFile,buf,sizeof(buf),&dwread,NULL);
		if(dwread>0)
			WriteFile(hWriteFile,buf,dwread,&dwread,NULL);
	} while(dwread>0);
	operation_ok = true;
clean_rename_file:
	if(hReadFile)
		CloseHandle(hReadFile);
	if(hWriteFile)
	{
		CloseHandle(hWriteFile);
	}
	return operation_ok;
}
void rename_file()
{
	LPCTSTR cmdline = GetCommandLine();
	LPCTSTR pidpos = _tcsstr(cmdline,_T("-PID"));
	LPCTSTR newnamepos = _tcsstr(cmdline,_T("-name"));
	TCHAR buf[MAX_PATH+1024];
	DWORD dwret = GetModuleFileName(NULL,buf,sizeof(buf)/sizeof(buf[0]) );
	bool copy_ok = false;
	HANDLE hprocess = NULL;
	if(pidpos && newnamepos )
	{
		pidpos = pidpos+4;
		newnamepos = newnamepos+5;
		int tmp = atoi(pidpos);
		hprocess = OpenProcess(SYNCHRONIZE,FALSE,tmp);
		WaitForSingleObject(hprocess,1000*100);
		if(hprocess)
			CloseHandle(hprocess);
		if(!DeleteFile(newnamepos))
			ExitProcess(0);
		copy_ok = copy_file(buf,newnamepos);
		if(copy_ok)
		{
			SelfDelete();
		}
		ExitProcess(0);

	}
}

#endif
