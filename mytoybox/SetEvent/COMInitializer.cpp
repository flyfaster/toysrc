#include <Windows.h>
#include <string>
#include <sstream>
#include <tchar.h>
#include "COMInitializer.h"

std::basic_string<TCHAR> ToString( VARTYPE vt )
{
    std::basic_string<TCHAR> ret;
    if (vt & VT_VECTOR)
        ret = ret + _T("VT_VECTOR | ");
    if (vt & VT_ARRAY)
        ret = ret + _T("VT_ARRAY | ");
    if (vt & VT_BYREF)
        ret = ret + _T("VT_BYREF | ");
    if (vt & VT_RESERVED)
        ret = ret + _T("VT_RESERVED | ");

    if (vt > VT_VECTOR)
    {
        return ret + ToString( vt & 0xFFF);
    }
    switch(vt)
    {
    case VT_EMPTY: return _T("VT_EMPTY"); break;
    case VT_NULL: return _T("VT_NULL"); break;
    case VT_I2: return _T("VT_I2"); break;
    case VT_I4: return _T("VT_I4"); break;
    case VT_R4: return _T("VT_R4"); break;
    case VT_R8: return _T("VT_R8"); break;
    case VT_CY: return _T("VT_CY"); break;
    case VT_DATE: return _T("VT_DATE"); break;
    case VT_BSTR: return _T("VT_BSTR"); break;
    case VT_DISPATCH: return _T("VT_DISPATCH"); break;
    case VT_ERROR: return _T("VT_ERROR"); break;
    case VT_BOOL: return _T("VT_BOOL"); break;
    case VT_VARIANT: return _T("VT_VARIANT"); break;
    case VT_UNKNOWN: return _T("VT_UNKNOWN"); break;
    case VT_DECIMAL: return _T("VT_DECIMAL"); break;
    case VT_I1: return _T("VT_I1"); break;
    case VT_UI1: return _T("VT_UI1"); break;
    case VT_UI2: return _T("VT_UI2"); break;
    case VT_UI4: return _T("VT_UI4"); break;
    case VT_I8: return _T("VT_I8"); break;
    case VT_UI8: return _T("VT_UI8"); break;
    case VT_INT: return _T("VT_INT"); break;
    case VT_UINT: return _T("VT_UINT"); break;
    case VT_VOID: return _T("VT_VOID"); break;
    case VT_HRESULT: return _T("VT_HRESULT"); break;
    case VT_PTR: return _T("VT_PTR"); break;
    case VT_SAFEARRAY: return _T("VT_SAFEARRAY"); break;
    case VT_CARRAY: return _T("VT_CARRAY"); break;
    case VT_USERDEFINED: return _T("VT_USERDEFINED"); break;
    case VT_LPSTR: return _T("VT_LPSTR"); break;
    case VT_LPWSTR: return _T("VT_LPWSTR"); break;
    case VT_RECORD: return _T("VT_RECORD"); break;
    case VT_INT_PTR: return _T("VT_INT_PTR"); break;
    case VT_UINT_PTR: return _T("VT_UINT_PTR"); break;
    case VT_FILETIME: return _T("VT_FILETIME"); break;
    case VT_BLOB: return _T("VT_BLOB"); break;
    case VT_STREAM: return _T("VT_STREAM"); break;
    case VT_STORAGE: return _T("VT_STORAGE"); break;
    case VT_STREAMED_OBJECT: return _T("VT_STREAMED_OBJECT"); break;
    case VT_STORED_OBJECT: return _T("VT_STORED_OBJECT"); break;
    case VT_BLOB_OBJECT: return _T("VT_BLOB_OBJECT"); break;
    case VT_CF: return _T("VT_CF"); break;
    case VT_CLSID: return _T("VT_CLSID"); break;
    case VT_VERSIONED_STREAM: return _T("VT_VERSIONED_STREAM"); break;
    case VT_BSTR_BLOB: return _T("VT_BSTR_BLOB"); break;
    default: return _T("Invalid type"); break;
    }
}

COMInitializer::COMInitializer( DWORD dwCoInit /*= COINIT_APARTMENTTHREADED*/ ) : m_init_thread(0)
{
    m_hr = CoInitializeEx(NULL, dwCoInit); 
    if (!Error())
    {
        m_init_thread = GetCurrentThreadId();
    }
}

COMInitializer::~COMInitializer()
{
    if (SUCCEEDED(m_hr) && GetCurrentThreadId()==m_init_thread)
    {
        CoUninitialize();
    }
}

bool COMInitializer::Error()
{
    return (FAILED(m_hr) && (RPC_E_CHANGED_MODE!=m_hr));
}

DWORD COMInitializer::GetThreadId()
{
    return m_init_thread;
}