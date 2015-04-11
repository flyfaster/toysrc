// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2DBC7217_65E6_49DA_8EEB_856AE7A7DB15__INCLUDED_)
#define AFX_STDAFX_H__2DBC7217_65E6_49DA_8EEB_856AE7A7DB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afx.h>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#ifndef __FUNCTION__
#define __FUNCTION__ __FILE__
#endif
#define __WFILE__ WIDEN(__FILE__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)
#ifdef _UNICODE
#define __TFILE__ __WFILE__
#define __TFUNCTION__ __WFUNCTION__
#else
#define __TFILE__ __FILE__
#define __TFUNCTION__ __FUNCTION__
#endif
#define ARRAY_SIZE(X)   sizeof(X)/sizeof(X[0])
#define WHERE_AM_I
#define TRACK_CONSTRUCTOR
#define TRACK_DESTRUCTOR
//#define WHERE_AM_I {TCHAR buf_where_am_i[MAX_PATH+256];wsprintf(buf_where_am_i,_T("%s<%d>,threadid:%d,process ID:%d"),__TFUNCTION__,__LINE__,GetCurrentThreadId(),GetCurrentProcessId());OutputDebugString(buf_where_am_i);}
//#define TRACK_CONSTRUCTOR {InterlockedIncrement(&mg_lInstanceCount);	AtlTrace( _T("%s instance count %d"),__TFUNCTION__, mg_lInstanceCount); }
//#define TRACK_DESTRUCTOR {InterlockedDecrement(&mg_lInstanceCount); AtlTrace(_T("%s instance count %d"),__TFUNCTION__, mg_lInstanceCount);}

#endif // !defined(AFX_STDAFX_H__2DBC7217_65E6_49DA_8EEB_856AE7A7DB15__INCLUDED_)
