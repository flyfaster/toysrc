// stdafx.cpp : source file that includes just the standard includes
// stockclient.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
//#ifndef _CONSOLE
//STOCKCLIENT_API basic_dostream<char>    cerr;
//STOCKCLIENT_API basic_dostream<char>    cout;
//STOCKCLIENT_API basic_dostream<wchar_t> wcerr;
//STOCKCLIENT_API basic_dostream<wchar_t> wcout;
//#endif


#if defined STOCKCLIENTDLL_EXPORTS

HANDLE g_hDll = NULL;

BOOL WINAPI DllMain(HANDLE hDll, DWORD  dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hDll = hDll;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:

		break;

	}

	return TRUE;
}
#endif