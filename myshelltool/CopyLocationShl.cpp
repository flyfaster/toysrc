// CopyLocationShl.cpp : Implementation of CCopyLocationShl
#include "stdafx.h"
#include <atlconv.h>  // for ATL string conversion macros 
//http://www.codeproject.com/shell/copylocation.asp
//http://www.codeproject.com/tips/dirclean.asp#xx380134xx

#include "CopyLocation.h"
#include "CopyLocationShl.h"
#include <sstream>
#include "SheelToolConfig.h"
/////////////////////////////////////////////////////////////////////////////
// CCopyLocationShl

CCopyLocationShl::CCopyLocationShl()
{
	m_hCopyBmp = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_COPY));
}


HRESULT CCopyLocationShl::Initialize(
    LPCITEMIDLIST pidlFolder,
    LPDATAOBJECT  pDataObj,
    HKEY          hProgID
	)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;
    // Look for CF_HDROP data in the data object
    if (FAILED(pDataObj->GetData(&fmt, &stg)))
	{
        // Nope! Return an "invalid argument" error back to Explorer
        return E_INVALIDARG;
	}
    // Get a pointer to the actual data
    hDrop = (HDROP)GlobalLock(stg.hGlobal);
    if (NULL == hDrop)
	{
        ReleaseStgMedium(&stg);
        return E_INVALIDARG;
	}
    // Sanity check	- make sure there is at least one filename
	UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    if (0 == uNumFiles)
	{
        GlobalUnlock(stg.hGlobal);
        ReleaseStgMedium(&stg);
        return E_INVALIDARG;
	}
	TCHAR szFile[MAX_PATH];
    for (UINT uFile = 0 ; uFile < uNumFiles ; uFile++)
	{
        // Get the next filename.
        if (0 == DragQueryFile(hDrop, uFile, szFile, MAX_PATH))
            continue;
        // Add the file name to our list of files (m_lsFiles).
        m_lsFiles.push_back(szFile);
	}
    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);
    // If we found any files we can work with, return S_OK. Otherwise, return
    // E_INVALIDARG so we don't get called again for this right-click operation.
    return (m_lsFiles.size() > 0) ? S_OK : E_INVALIDARG;
}

LPCTSTR szMenuText_Popup		= _T("Onega's Tools-->");
LPCTSTR szMenuText_RemoveTmpFiles = _T("Remove VC temporary files");
LPCTSTR szMenuText_RegisterOCX =_T("Register COM DLL");
LPCTSTR szMenuText_UnregisterOCX=_T("Unregister COM DLL");
LPCTSTR szMenuText_CopyCPP_Path = _T("Copy CPP path");
LPCTSTR szMenuText_Disable_ReadOnly = _T("Disable Readonly property");
LPCTSTR szMenuText_Copy_Filename_No_Ext = _T("Copy file name without extension");
LPCTSTR szMenuText_VC60_Build = _T("Build with VC60");
LPCTSTR szMenuText_VC71_Build = _T("Build with VC71");
LPCTSTR szMenuText_Options = _T("Options...");
LPCTSTR szMenuText_RunCommand = _T("Run command");
LPCTSTR szMenuText_CopyFileName = _T("Copy file name");
LPCTSTR szMenuText_CopyPathName = _T("Copy path name");

HRESULT CCopyLocationShl::QueryContextMenu(
    HMENU hMenu,
    UINT  uMenuIndex, 
    UINT  uidFirstCmd,
    UINT  uidLastCmd,
    UINT  uFlags
	)
{
	//_UNUSED_PARAMETER(uidLastCmd);
	UINT idCmd = uidFirstCmd;
	BOOL bAppendItems = TRUE;
	if((uFlags & 0x000F) == CMF_NORMAL)
		bAppendItems = TRUE;
	else if (uFlags & CMF_VERBSONLY)
		bAppendItems = TRUE;
	else if (uFlags & CMF_EXPLORE)
		bAppendItems = TRUE;
	else
		bAppendItems = FALSE;

	if(bAppendItems)
	{
		InsertMenu(hMenu, uMenuIndex, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);  
		uMenuIndex++;
		HMENU hSubMenu = CreateMenu();
		if(hSubMenu)
		{
			int nSubMenuIndex = 0;
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING	 | MF_BYPOSITION, idCmd++, szMenuText_CopyPathName);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING	 | MF_BYPOSITION, idCmd++, szMenuText_CopyFileName);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING	 | MF_BYPOSITION, idCmd++, szMenuText_RunCommand);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);  
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_RemoveTmpFiles);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);  
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_RegisterOCX);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_UnregisterOCX);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);  
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_CopyCPP_Path);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_Disable_ReadOnly);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING|MF_BYPOSITION, idCmd++,szMenuText_Copy_Filename_No_Ext);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING | MF_BYPOSITION, idCmd++, szMenuText_VC60_Build);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING | MF_BYPOSITION, idCmd++, szMenuText_VC71_Build);
			InsertMenu(hSubMenu, nSubMenuIndex++, MF_STRING | MF_BYPOSITION, idCmd++, szMenuText_Options);
		}
		InsertMenu(hMenu, uMenuIndex, MF_STRING | MF_POPUP | MF_BYPOSITION, (UINT_PTR)hSubMenu, szMenuText_Popup);
		uMenuIndex++;
		InsertMenu(hMenu, uMenuIndex, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);  
		uMenuIndex++;
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - uidFirstCmd);
	}
	return NOERROR;
}
const long COPY_FULL_PATH = 0;
const long COPY_FILE_NAME = 1;
const long RUN_COMMAND = 2;
const long REMOVE_VC_TEMP_FILES = 3;
const long REGISTER_DLL = 4;
const long UNREGISTER_DLL = 5;
const long COPY_CPP_PATH = 6;
const long DISABLE_FILE_READONLY = 7;
const long COPY_FILE_NAME_NO_EXT = 8;
const long VC60_BUILD = 9;
const long VC71_BUILD = 10;
const long MENU_OPTIONS = 11;

HRESULT CCopyLocationShl::GetCommandString(
    UINT  idCmd,
    UINT  uFlags,
    UINT* pwReserved,
    LPSTR pszName,
    UINT  cchMax
	)
{
	LPCTSTR szPrompt;
    USES_CONVERSION;
    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer
    if (uFlags & GCS_HELPTEXT)
	{
		switch (idCmd)
		{//The string would be displayed in status bar of explorer.
		case COPY_FULL_PATH:  szPrompt = _T("Copy the full file path(s) to the clipboard"); break;
		case COPY_FILE_NAME:  szPrompt = _T("Copy the file name(s) to the clipboard");      break;
		case RUN_COMMAND: szPrompt=_T("Run command at this directory"); break;
		case REMOVE_VC_TEMP_FILES: szPrompt=_T("Remove temporary VC output files"); break;
		case COPY_CPP_PATH: szPrompt = _T("Copy full path in C++ format"); break;
		case DISABLE_FILE_READONLY: szPrompt = szMenuText_Disable_ReadOnly; break;
		case VC60_BUILD: szPrompt = szMenuText_VC60_Build; break;
		case VC71_BUILD: szPrompt = szMenuText_VC71_Build; break;
		case MENU_OPTIONS: szPrompt = szMenuText_Options; break;
		default: return E_INVALIDARG;
		}
        if (uFlags & GCS_UNICODE)
		{
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API
            lstrcpynW((LPWSTR)pszName, T2CW(szPrompt), cchMax);
		}
        else
		{
            // Use the ANSI string copy API to return the help string
            lstrcpynA(pszName, T2CA(szPrompt), cchMax);
		}

        return S_OK;
	}

    return E_INVALIDARG;
}

tchar_string to_cpp_path(const tchar_string file_path)
{
	std::basic_stringstream<TCHAR> ts;
	TCHAR slash = _T('\\');
	for(size_t i=0; i<file_path.length(); i++)
	{
		TCHAR c = file_path[i];
		if(c == slash)
		{
			ts<<c<<c;
		}
		else
			ts<<c;
	}
	return ts.str();
}
extern DWORD use_unicode;
HRESULT copy_to_clipboard(LPCMINVOKECOMMANDINFO pCmdInfo,tchar_string& clipBuffer)
{
	if(!pCmdInfo)
		return S_OK;
	if (!OpenClipboard(pCmdInfo->hwnd))
		return S_OK;
	EmptyClipboard();
	HGLOBAL hCom = NULL;
	LPVOID pCom = NULL;
	if(0 == use_unicode)
	{
		long size = clipBuffer.size() + 1;
		if (!(hCom = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size)))
			return S_OK;
		if (!(pCom = GlobalLock(hCom)))
		{
			GlobalFree(hCom);
			return S_OK;
		}
		_bstr_t bstmp = clipBuffer.c_str();
		CopyMemory(pCom, (LPCSTR)bstmp, size);
		SetClipboardData(CF_TEXT, hCom);
	}
	else
	{
		long size = clipBuffer.size() + 1;
		size = size*2;
		if (!(hCom = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size)))
			return S_OK;
		if (!(pCom = GlobalLock(hCom)))
		{
			GlobalFree(hCom);
			return S_OK;
		}
		_bstr_t bstmp = clipBuffer.c_str();
		CopyMemory(pCom, (LPCWSTR)bstmp, size);
		SetClipboardData(CF_UNICODETEXT, hCom);		
	}
	GlobalUnlock(hCom);
	CloseClipboard();	
	return S_OK;
}
BOOL DoesFileExist(LPCTSTR pszFile)
{
	HANDLE    hFile = NULL ;
	// We don't want any error message box to pop up when we try to test
	// if the required file is available to open for read.

	UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS | 
		SEM_NOOPENFILEERRORBOX);
	hFile = CreateFile(pszFile, GENERIC_READ, 
		FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	SetErrorMode(uErrorMode);  // restore error mode
	if (INVALID_HANDLE_VALUE  == hFile) 
		return FALSE ;    

	CloseHandle(hFile);
	return TRUE;
} //End function DoesFileExist
void build_by_vc(LPCTSTR long_source_file_name,
				 LPCTSTR REG_VC6_ROOT,
				 LPCTSTR remove_part,
				 LPCTSTR var_part,
				 LPCTSTR build_file_name,
				 bool show_cmd = false,
				 bool delete_bat = true)
{
	TCHAR source_file_name[MAX_PATH + 1024];
	source_file_name[0] = 0;
	//DWORD file_attributes = GetFileAttributes(long_source_file_name);
	//ignore non cpp file:
	std::basic_string<TCHAR> tmp = long_source_file_name;
	//static ToUpper up(std::locale::classic());
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);	
	size_t pos1 = tmp.rfind(_T("."));
	size_t pos2 = tmp.rfind(_T(".CPP"));
	if(false == (pos1>0 && pos1<tmp.length() && pos1 == pos2))
	{
		//AfxMessageBox(_T("not cpp file"));
		return;
	}
	//GetShortPathName(long_source_file_name,source_file_name,sizeof(source_file_name)/sizeof(source_file_name[0]));
	_tcscpy(source_file_name, long_source_file_name);
	CRegKey reg;
	TCHAR install_dir_buf[MAX_PATH + 1024];
	std::basic_string<TCHAR> fullpath;
	if( ERROR_SUCCESS == reg.Open(HKEY_LOCAL_MACHINE,REG_VC6_ROOT, KEY_READ ) )
	{
		install_dir_buf [ 0] =0;
		ULONG dir_length = sizeof(install_dir_buf)/sizeof(install_dir_buf[0]);
		reg.QueryStringValue(_T("InstallDir"),install_dir_buf, &dir_length );
		if(dir_length>0)
		{
			fullpath = install_dir_buf;
			size_t pos = fullpath.find(remove_part);
			if(pos >0 && pos < fullpath.length())
				fullpath = fullpath.substr(0,pos);
			fullpath += var_part;
		}
	}
	reg.Close();
	//set work dir to path of source file
	std::basic_string<TCHAR> workdir = source_file_name;
	size_t pos = workdir.rfind(_T("\\"));
	if(pos>0 && pos< workdir.length())
	{
		workdir = workdir.substr(0, pos+1);
	}
	std::basic_string<TCHAR> build_file_path = workdir;
	build_file_path += build_file_name;
	std::basic_ofstream<TCHAR> ofile;
	ofile.open((LPCSTR)_bstr_t(build_file_path.c_str()));
	//del exe file	
	//if exist "c:\temp\t.exe" goto exists
	//	echo File not found
	//	goto end
	//	:exists
	//	del "c:\temp\t.exe"
	//	:end
	std::basic_string<TCHAR> objfile = source_file_name;
	pos = objfile.rfind(_T("."));
	if(pos>0 && pos<objfile.length())
	{
		objfile = objfile.substr(0,pos);
	}
	else
		objfile = _T("");
	
	if(objfile.length()>0 )
	{
		std::basic_string<TCHAR> exefile = objfile;
		exefile += _T(".exe");
		if(DoesFileExist(exefile.c_str()))
			ofile<<_T("del \"")<<exefile <<_T("\"")<<std::endl;
	}
	ofile<<_T("call \"")<<fullpath<<_T("\"")<<std::endl;
	ofile<<_T("cl /TP \"")<<source_file_name<<_T("\"")<<std::endl;
	//delete obj file
	if(objfile.length()>0)
	{
		std::basic_string<TCHAR> objfilename = objfile;
		objfilename += _T(".obj");
		ofile << _T("if exist \"")<<objfilename<<_T("\" goto exists")<<std::endl;
		ofile<<_T("goto end")<<std::endl;
		ofile<<_T(":exists")<<std::endl;
		ofile<<_T("del \"")<<objfilename<<_T("\"")<<std::endl;
		ofile<<_T(":end")<<std::endl;
	}
	if(show_cmd)	
		ofile<<_T("pause")<<std::endl;
	ofile.close();
	ShellExecute(NULL, _T("open"), build_file_path.c_str(), NULL, workdir.c_str(), SW_SHOWNORMAL);
	if(delete_bat)
		DeleteFile(build_file_path.c_str());
}

//////////////////////////////////////////////////////////////////////////
//http://www.mingw.org/docs.shtml
//create a console mode executable hello.exe from a c file called hello.c
//gcc -o hello hello.c
//For the C++ program, use the following to compile and link:
//g++ -c hello.cpp
//g++ -o hello hello.o
//create a Windows executable hello.exe, from a c file called hello.c
//gcc -c hello.c
//gcc -o hello hello.o -mwindows
//[HKEY_LOCAL_MACHINE\SOFTWARE\Dev-C++]
//Install_Dir = "d:\apps\Dev-Cpp"

HRESULT CCopyLocationShl::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
    // If lpVerb really points to a string, ignore this function call and bail out
    if (0 != HIWORD(pCmdInfo->lpVerb))
        return E_INVALIDARG;
    // Get the command index - the only valid commands are 0 and 1 , 3
	WORD wCmd = LOWORD(pCmdInfo->lpVerb);
	//if (wCmd > 2)
	//	return E_INVALIDARG;
	string_list::const_iterator it, itEnd;
	tchar_string clipBuffer;
	tchar_string name;
	it = m_lsFiles.begin();
	itEnd = m_lsFiles.end();
	switch (wCmd)
	{
	case COPY_FULL_PATH:
		clipBuffer = it->c_str();
		break;
	case COPY_FILE_NAME:
		CopyFileName(name, *it);
    	clipBuffer = name;
		break;
	case RUN_COMMAND:
		RunCmd(pCmdInfo);
		return S_OK;
	case REMOVE_VC_TEMP_FILES:
		return RemoveVCTmpFiles(pCmdInfo);
		break;
	case REGISTER_DLL:
		RegisterDLL(pCmdInfo,true);
		break;
	case UNREGISTER_DLL:
		RegisterDLL(pCmdInfo,false);
		break;
	case COPY_CPP_PATH:
		clipBuffer = to_cpp_path(*it);
		break;
	case DISABLE_FILE_READONLY:
		for(;it!=itEnd;it++)
		{
			tchar_string filename = *it;
			DWORD file_attributes = GetFileAttributes(filename.c_str());
			if(file_attributes & FILE_ATTRIBUTE_READONLY)
			{
				file_attributes = (file_attributes ^ FILE_ATTRIBUTE_READONLY);
				SetFileAttributes(filename.c_str(),file_attributes);
			}
		}
		return ERROR_SUCCESS;
	case COPY_FILE_NAME_NO_EXT:
		{
			std::basic_stringstream<TCHAR> ts;
			for(;it!=itEnd;it++)
			{
				tchar_string filename;
				CopyFileName(filename, *it);
				filename = filename.substr(0,filename.rfind(_T('.')));
				ts<<filename<<std::endl;
			}
			copy_to_clipboard(pCmdInfo,ts.str());
		}
		return ERROR_SUCCESS;
	case VC60_BUILD:
		{
			tchar_string source_file = *it;
			LPCTSTR REG_VC60_ROOT = _T("SOFTWARE\\Microsoft\\VisualStudio\\6.0");
			LPCTSTR remove_part = _T("Common\\IDE\\IDE98");
			LPCTSTR var_part = _T("VC98\\Bin\\VCVARS32.BAT");
			LPCTSTR build_file_name = _T("tmp_build_vc60.bat");
			///////////////////////////////////////////////////////
			build_by_vc(source_file.c_str(), REG_VC60_ROOT, remove_part, 
				var_part, build_file_name, show_build_console, 0==keep_build_script);

		}
		return ERROR_SUCCESS;
	case VC71_BUILD:
		{
			tchar_string source_file = *it;
			LPCTSTR REG_VC71_ROOT = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1");
			LPCTSTR remove_part = _T("Common7\\IDE");
			LPCTSTR var_part = _T("Vc7\\bin\\vcvars32.bat");
			LPCTSTR build_file_name = _T("tmp_build_vc71.bat");
			build_by_vc(source_file.c_str(), REG_VC71_ROOT, remove_part, 
				var_part, build_file_name,show_build_console, 0==keep_build_script);
		}
		return ERROR_SUCCESS;
	case MENU_OPTIONS:
		{
			CSheelToolConfig config_dialog;
			config_dialog.DoModal();
		}
		return ERROR_SUCCESS;
	default:
		return E_INVALIDARG;
	}

    while (++it != itEnd)
	{
		clipBuffer += _T("\r\n");
		switch (wCmd)
		{
		case COPY_FULL_PATH:
			clipBuffer += it->c_str();
			break;
		case COPY_FILE_NAME:
			CopyFileName(name, *it);
    		clipBuffer += name;
			break;
		case COPY_CPP_PATH:
			clipBuffer += to_cpp_path(*it);
			break;
		}
	}
	copy_to_clipboard(pCmdInfo,clipBuffer);
	return S_OK;
}


void CCopyLocationShl::CopyFileName(tchar_string &name, const tchar_string &fullpath)
{
	long pos = fullpath.find_last_of(_T('\\'));
	name.assign(fullpath, pos + 1, fullpath.size() - pos - 1);
}

HRESULT CCopyLocationShl::RunCmd(LPCMINVOKECOMMANDINFO lpcmi)
{
	TCHAR msgbuf[10240];
	memset(msgbuf,0,sizeof(msgbuf));;
	if(m_lsFiles.size()>0)
	{
		TCHAR cCmdBuf[1024];
		memset(cCmdBuf,0,sizeof(cCmdBuf));
		tchar_string first_file = m_lsFiles.front();
		const TCHAR* pend = m_lsFiles.front().c_str();
		wsprintf(msgbuf,_T("RunCmd filename %s"),pend);//OutputDebugString(msgbuf);
		if(PathIsDirectory(pend))
		{
			_tcscpy(cCmdBuf,pend);
		}
		else
		{
			size_t pos = first_file.find_last_of(_T('\\'));
			if(pos!=tchar_string::npos)
			{
				first_file = first_file.substr(0,pos);
				_tcscpy(cCmdBuf, first_file.c_str());
			}
			else
				return S_OK;
		}
		wsprintf(msgbuf,_T("pathname: %s"),cCmdBuf); //OutputDebugString(msgbuf);
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si,0,sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = 0;
		si.cbReserved2 = 0;
		si.lpReserved2 = NULL;
		memset(&pi,0,sizeof(pi));
		TCHAR cmd_buf[MAX_PATH];
		memset(cmd_buf,0,sizeof(cmd_buf));
		_tcscpy(cmd_buf,_T("cmd.exe"));
		BOOL bres = CreateProcess(
			NULL,
			cmd_buf,
			NULL,
			NULL,
			false,
			NORMAL_PRIORITY_CLASS,
			NULL,
			cCmdBuf,
			&si,
			&pi);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	return S_OK;
}

HRESULT CCopyLocationShl::RemoveVCTmpFiles(LPCMINVOKECOMMANDINFO lpcmi)
{
	//delete *.obj, *.pch, *.pdb, *.res, *.idb, *.ilk
	string_list::const_iterator it, itEnd;
	tchar_string clipBuffer;
	tchar_string name;
	it = m_lsFiles.begin();
	itEnd = m_lsFiles.end();
	for(;it!= itEnd;it++)
	{
		if(PathIsDirectory(it->c_str()))
		{
			ClearVCDir(it->c_str());
		}
	}
	return S_OK;
}
bool end_with(std::basic_string<TCHAR> s,std::basic_string<TCHAR> postfix,bool ignore_case=true)
{
	if(ignore_case)
	{
		std::transform(s.begin(),s.end(),s.begin(),toupper);
		std::transform(postfix.begin(),postfix.end(),postfix.begin(),toupper);
	}

	size_t postfix_length = postfix.length();
	size_t first_str_pos = s.length()-postfix.length();
	if(s.compare(first_str_pos,postfix_length,postfix)==0)
		return true;
	else
		return false;
}
int CCopyLocationShl::ClearVCDir(LPCTSTR pszPath)
{
	if(!PathIsDirectory(pszPath) || pszPath==NULL)
		return 0;
	tchar_string sPath=pszPath;
	if(pszPath[sPath.length()-1]!= _T('\\'))
		sPath+=_T("\\");
	tchar_string sGoodPath = sPath; // end with \
	//LPCTSTR filename = sPath.c_str();
	sPath+=_T("*.*");
	TCHAR msgbuf[2048];
	wsprintf(msgbuf,_T("%s:%s deal with %s"),__TFILE__,__TFUNCTION__,sPath.c_str());
	//OutputDebugString(msgbuf);

		WIN32_FIND_DATA wfd= {0};
		HANDLE  hFind = FindFirstFile(sPath.c_str(), &wfd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			if(enable_dbg_output)
			OutputDebugString(_T("hFind == INVALID_HANDLE_VALUE"));
			return 0;
		}
		do
		{
			if(wfd.cFileName[0]==_T('.'))
				continue;
			TCHAR allname[MAX_PATH];
			_tcscpy(allname,sGoodPath.c_str());
			_tcscat(allname,wfd.cFileName);
			wsprintf(msgbuf,_T("%s:%s allname %s"),__TFILE__,__TFUNCTION__,allname);
			//OutputDebugString(msgbuf);
			if((GetFileAttributes(allname) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				ClearVCDir(allname);
			}
			else
			{
				if(IsTmpFile(allname))
				{
					BOOL bret = DeleteFile(allname);
					if(!bret)
					{//try to delete read-only file
						SetFileAttributes(allname,FILE_ATTRIBUTE_NORMAL);
						DeleteFile(allname);
					}
				}
			}
		}while(FindNextFile(hFind, &wfd));
		FindClose(hFind);	return 0;
}

bool CCopyLocationShl::IsTmpFile(LPCTSTR pszFileName)
{
	//delete *.obj, *.pch, *.pdb, *.res, *.idb, *.ilk
	LPCTSTR pcszExtensions[]={_T(".obj"),_T(".pch"),_T(".pdb"),_T(".idb"),
		_T(".ilk"),_T(".res"),_T(".ncb"),_T(".aps"),_T(".sbr"),_T(".exp"),_T(".tlh"),
		_T(".tli"),_T(".bsc"),_T(".aps")
	};	TCHAR msgbuf[2048];
	wsprintf(msgbuf,_T("%s:%s pszFileName %s"),__TFILE__,__TFUNCTION__,pszFileName);
	for(int i=0;i<sizeof(pcszExtensions)/sizeof(pcszExtensions[0]);i++)
	{
		if(end_with(pszFileName,pcszExtensions[i]))
			return true;
	}	
	return false;
}

HRESULT CCopyLocationShl::RegisterDLL(LPCMINVOKECOMMANDINFO lpcmi, bool bRegister)
{
	HRESULT hResult = S_OK; 
	TCHAR msgbuf[1024];
	if(m_lsFiles.size()==1)
	{
		const TCHAR* pend = m_lsFiles.front().c_str();
		wsprintf(msgbuf,_T("%s filename %s"),__TFUNCTION__,pend);//OutputDebugString(msgbuf);
		if(PathIsDirectory(pend))
		{
			return S_OK;
		}
		HINSTANCE hOleServerInst = NULL; 
		// Load the server dll into our process space. 
		hOleServerInst = ::LoadLibrary(pend); 
		if (hOleServerInst) 
		{ 
			HRESULT (STDAPICALLTYPE *pfnRegServer)(void); 
			if (bRegister) 
			{ 
				(FARPROC&)pfnRegServer = ::GetProcAddress(hOleServerInst, ("DllRegisterServer")); 
			} 
			else 
			{ 
				(FARPROC&)pfnRegServer = ::GetProcAddress(hOleServerInst, ("DllUnregisterServer")); 

			} 
			if (pfnRegServer) 
			{ 
				hResult = (*pfnRegServer)(); 
			} 
			else 
			{ 
				hResult = HRESULT_FROM_WIN32(::GetLastError()); 
			} 
			::CoFreeLibrary(hOleServerInst); 
		} 
		else 
		{ 
			hResult = HRESULT_FROM_WIN32(::GetLastError()); 
		}
		if(bRegister && S_OK==hResult)
			_tcscpy(msgbuf,_T("DllRegisterServer succeeded"));
		else
			wsprintf(msgbuf,_T("DllUnregisterServer return %08x"),hResult);
		MessageBox(NULL,msgbuf,_T("Result"),MB_OK);
		return hResult;
	}
	return S_OK;
}
