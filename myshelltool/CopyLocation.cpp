// CopyLocation.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f CopyLocationps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "CopyLocation.h"

#include "CopyLocation_i.c"
#include "CopyLocationShl.h"
#include <sstream>
#include "get_link_info.h"
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_CopyLocationShl, CCopyLocationShl)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

LPCTSTR REG_APPLICATION_ROOT = _T("SOFTWARE\\www.fruitfruit.com\\shelltool");
LPCTSTR ENABLED_OPT = _T("Enabled");
LPCTSTR USE_UNICODE_OPT = _T("UseUnicode");
DWORD use_unicode = 0;
DWORD enabled = 1;
DWORD keep_build_script = 1;
DWORD enable_dbg_output = 0;
DWORD show_build_console = 1;
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	dump_module_link_info(hInstance);
	CRegKey reg;
	LONG lret = reg.Open(HKEY_LOCAL_MACHINE,REG_APPLICATION_ROOT,KEY_READ);
	if(ERROR_SUCCESS == lret)
	{
		reg.QueryDWORDValue(ENABLED_OPT,enabled);
		reg.QueryDWORDValue(_T("UseUnicode"), use_unicode);
		reg.QueryDWORDValue(_T("KeepBuildScript"),keep_build_script);
		reg.QueryDWORDValue(_T("EnableTraceOutput"),enable_dbg_output);
		reg.QueryDWORDValue(_T("ShowBuildConsole"), show_build_console);
	}
	reg.Close();
	TCHAR buf[MAX_PATH+1024];
	buf[0] = 0;
	GetModuleFileName(hInstance, buf, sizeof(buf)/sizeof(buf[0]));
	std::basic_stringstream<TCHAR> ts;
	ts<<buf<<" ";
	ts<<std::endl;
	ts<<_T("[HKLM\\")<<REG_APPLICATION_ROOT<<_T("] Enabled = ")<<enabled<<std::endl;
	ts<<_T("[HKLM\\")<<REG_APPLICATION_ROOT<<_T("] UseUnicode = ")<<use_unicode<<std::endl;
	ts<<_T("[HKLM\\")<<REG_APPLICATION_ROOT<<_T("] KeepBuildScript = ")<<keep_build_script<<std::endl;
	ts<<_T("[HKLM\\")<<REG_APPLICATION_ROOT<<_T("] EnableTraceOutput = ")<<enable_dbg_output<<std::endl;
	ts<<_T("[HKLM\\")<<REG_APPLICATION_ROOT<<_T("] ShowBuildConsole = ")<<show_build_console;
	if(enable_dbg_output)
	OutputDebugString(ts.str().c_str());
	if(0 == enabled)
	{
		return FALSE;
	}
	//return FALSE;
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_COPYLOCATIONLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // If we're on NT, add ourselves to the list of approved shell extensions
    if (0 == (GetVersion() & 0x80000000UL))
	{
		CRegKey reg;
		LONG    lRet;
		lRet = reg.Open(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
			KEY_SET_VALUE);
		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;
		lRet = reg.SetValue(_T("CopyLocationShl extension"), 
			_T("{A7847D3E-09F3-11D4-8F6D-0080AD87DD41}") );
        if (ERROR_SUCCESS != lRet)
            return E_ACCESSDENIED;
	}
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    // If we're on NT, remove ourselves from the list of approved shell extensions.
    // Note that if we get an error along the way, I don't bail out since I want
    // to do the normal ATL unregistration stuff too.
    if (0 == (GetVersion() & 0x80000000UL))
	{
        CRegKey reg;
        LONG    lRet;
        lRet = reg.Open(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
			KEY_SET_VALUE);
        if (ERROR_SUCCESS == lRet)
		{
            lRet = reg.DeleteValue(_T("CopyLocationShl extension"));
		}
	}

    return _Module.UnregisterServer(TRUE);
}


