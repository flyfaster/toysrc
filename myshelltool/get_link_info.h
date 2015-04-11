// get_link_info.h
// Date: 2005/7/26
// Author: Onega
// suggested usage: call get_link_info() in main/winmain/dllmain to dump 
// module properties
// purpose: check module properties so that we don't mix single threaded module with multiple
// threaded modules.
// Don't include this file before #include <afx.h>
#ifndef _ONEGA_GET_LINK_INFO_H__
#define _ONEGA_GET_LINK_INFO_H__
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
// the following instruction force VC verbose libraries it linked to.
#pragma comment(linker,"/VERBOSE:LIB")

inline LPCTSTR get_link_info()
{
	static TCHAR link_info_buf[1024] = {0};
	if(!link_info_buf[0])
	{
		TCHAR tmp_buf[64] = {0};
		//character set information
#ifdef _MBCS
		_tcscat(link_info_buf,_T("[Multibyte-character] "));
#else
#ifdef _UNICODE
		_tcscat(link_info_buf,_T("[Unicode (wide-character)] "));
#else
		_tcscat(link_info_buf,_T("[SBCS (ASCII)] "));
#endif
#endif	
		
#ifdef _MT
	_tcscat(link_info_buf, _T("[multi-threaded] "));
#else
	_tcscat(link_info_buf, _T("[single-threaded] "));
#endif
		
#ifdef _CPPUNWIND
		_tcscat(link_info_buf, _T("[Exception Handling enabled] "));
#else
		_tcscat(link_info_buf, _T("[Exception Handling disabled] "));
#endif
		
#ifdef _CPPRTTI
		_tcscat(link_info_buf, _T("[Enable Run-Time Type Information] "));
#else
		_tcscat(link_info_buf, _T("[Disable Run-Time Type Information] "));
#endif

#ifdef _DEBUG
		_tcscat(link_info_buf, _T("[debug] "));
#endif
#ifdef _NDEBUG
		_tcscat(link_info_buf, _T("[release] "));
#endif
#ifdef _MSC_EXTENSIONS
		_tcscat(link_info_buf, _T("[Enables Microsoft extensions] "));
#endif
		
#ifdef _DLL
		_tcscat(link_info_buf, _T("[Dynamic-link] "));
#else
		_tcscat(link_info_buf, _T("[Static-link] "));
#endif

#ifdef _M_IX86
		_tcscat(link_info_buf, _T("[_M_IX86] "));
#endif

#ifdef _NATIVE_WCHAR_T_DEFINED
		_tcscat(link_info_buf, _T("[_NATIVE_WCHAR_T_DEFINED] "));
#endif
#ifdef _CHAR_UNSIGNED
		_tcscat(link_info_buf, _T("[_CHAR_UNSIGNED] "));
#endif

#ifdef _MFC_VER
		_stprintf(tmp_buf,_T("[_MFC_VER=%d] "),_MFC_VER);
		_tcscat(link_info_buf, tmp_buf);
#endif
#ifdef _ATL_VER
		_stprintf(tmp_buf,_T("[_ATL_VER=%d] "),_ATL_VER);
		_tcscat(link_info_buf, tmp_buf);
#endif
#ifdef _MSC_VER
		_stprintf(tmp_buf,_T("[_MSC_VER=%d] "),_MSC_VER);
		_tcscat(link_info_buf, tmp_buf);
#endif
#ifdef _AFXDLL
		_tcscat(link_info_buf, _T("[Use MFC in a Shared DLL] "));
#endif
		
	}
	return link_info_buf;
}
// modulename should be DLL path name or exe file path name
// it dump link information of the module to debugger
// the output can be viewed in Dbgview.exe or \PlatformSDK\Bin\winnt\Dbmon.Exe
inline void dump_link_info(LPCTSTR modulename)
{
	TCHAR buf[1024+MAX_PATH];
	_stprintf(_T("%s %s"),modulename,get_link_info());
	OutputDebugString(buf);
}
// hInst should be handle of DLL or exe
// can pass NULL if it is called by exe
// purpose: dump module linking information to debugger
inline void dump_module_link_info(HINSTANCE hInst)
{
	TCHAR buf[1024+MAX_PATH];
	GetModuleFileName(hInst, buf, sizeof(buf)/sizeof(buf[0]));
	_tcscat(buf, get_link_info());
	OutputDebugString(buf);
}
#endif
