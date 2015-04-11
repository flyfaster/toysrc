// DLLVersion.h: interface for the CDLLVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLVERSION_H__88B3F8C1_27F2_11D3_80A3_0090276F9F55__INCLUDED_)
#define AFX_DLLVERSION_H__88B3F8C1_27F2_11D3_80A3_0090276F9F55__INCLUDED_
#pragma warning( disable : 4995 )
#include "shlwapi.h"
#pragma warning( default : 4995 )

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/*  For some odd reason, Microsoft published a sample code that uses shlwapi.h 
    (and shlwapi.lib)
    to tinker file versions.

    This header file could not be found anywhere !!!
    Not in Visual C++ 4.2, Visual C++ 5.0 or MSDN versions up to July 97`.

    So, I just took out the interesting structures from scraps I found 
    and re-defined them here.
*/


//  Remember: You must link version.lib to the project for this class to work !!


#ifndef _DLL_VERSION_H_
#define _DLL_VERSION_H_
//
//#ifndef DLLVERSIONINFO
//typedef struct _DllVersionInfo
//{
//    DWORD cbSize;
//    DWORD dwMajorVersion;
//    DWORD dwMinorVersion;
//    DWORD dwBuildNumber;
//    DWORD dwPlatformID;
//}DLLVERSIONINFO;
// 
//#endif


//#ifndef DLLVERSIONINFO
//typedef struct _DllVersionInfo
//{
//    DWORD cbSize;
//    DWORD dwMajorVersion;
//    DWORD dwMinorVersion;
//    DWORD dwBuildNumber;
//    DWORD dwPlatformID;
//}DLLVERSIONINFO;
//#endif


//#ifndef DLLGETVERSIONPROC
//typedef int (FAR WINAPI *DLLGETVERSIONPROC) (DLLVERSIONINFO *);
//#endif

class CDLLVersion
{
    typedef enum {  WIN_DIR, // Windows directory (e.g.: "C:\Windows\")
                    SYS_DIR, // Windows system directory (e.g.: "C:\Windows\System")
                    CUR_DIR, // Current directory (e.g.: ".")
                    NO_DIR}  // No directory (path in file name)
        FileLocationType;   // Possible ways to add a path prefix to a file

public:
	LPCTSTR ToString();
	
    CDLLVersion (LPTSTR szDLLFileName) :
        m_dwMajor (0),
        m_dwMinor (0),
        m_dwBuild (0)
    {
        m_bValid = GetDLLVersion (szDLLFileName, m_dwMajor, m_dwMinor, m_dwBuild);
    }
        

    virtual ~CDLLVersion () {};

    DWORD GetMajorVersion ()
    {
        return m_dwMajor;
    }

    DWORD GetMinorVersion ()
    {
        return m_dwMinor;
    }

    DWORD GetBuildNumber ()
    {
        return m_dwBuild;
    }

    BOOL IsValid ()
    {
        return m_bValid;
    }

	CString GetFullVersion()
	{
		return m_stFullVersion;
	}

private:
	TCHAR m_tcTextBuf[256];

	TCHAR *getVersion(TCHAR *fileName);
	
    BOOL GetDLLVersion (LPTSTR szDLLFileName, 
                        DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuildNumber);

    BOOL CheckFileVersion (LPTSTR szFileName, int FileLoc, 
                           DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuildNumber);

    BOOL ParseVersionString (LPTSTR lpVersion, 
                             DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuildNumber);
	BOOL ParseVersionString1 (LPTSTR lpVersion, 
                                      DWORD &dwMajor, DWORD &dwMinor, 
                                      DWORD &dwBuildNumber);

    BOOL FixFilePath (TCHAR * szFileName, int FileLoc);

    DWORD   m_dwMajor,      // Major version number
            m_dwMinor,      // Minor version number
            m_dwBuild;      // Build number
    BOOL    m_bValid;		// Is the DLL version information valid ?
	CString m_stFullVersion;
};

#endif

#endif // !defined(AFX_DLLVERSION_H__88B3F8C1_27F2_11D3_80A3_0090276F9F55__INCLUDED_)