// DLLVersion.cpp: implementation of the CDLLVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLVersion.h"
//#include "Global.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"version")
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/***************************************************************************

   Function:   GetDLLVersion

   Purpose:    Retrieves DLL major version, minor version and build numbers

   Input:      DLL file name
               Reference to Major number 
               Reference to Minor number 
               Reference to Build number 

   Output:     TRUE only if successful

   Remarks:    This function first tries to get the DLL version the nice way,
               that is, call the DllGetVersion function in the DLL.

               If this fails, it tries to located the DLL file in the file system,
               read the file information block and retrieve the file version.

****************************************************************************/
BOOL CDLLVersion::GetDLLVersion (LPTSTR szDLLFileName, 
                                 DWORD &dwMajor, DWORD &dwMinor, DWORD &dwBuildNumber)
{
HINSTANCE   hDllInst;           // Instance of loaded DLL
TCHAR szFileName [_MAX_PATH];    // Temp file name

BOOL bRes = TRUE;               // Result


    StringCchCopy (szFileName, sizeof(szFileName)/sizeof(szFileName[0]), szDLLFileName);    // Save a file name copy for the loading
    
	
	hDllInst = LoadLibrary(szFileName);   //load the DLL


    if(hDllInst) {  // Could successfully load the DLL
        DLLGETVERSIONPROC pDllGetVersion;
        /*
        You must get this function explicitly because earlier versions of the DLL 
        don't implement this function. That makes the lack of implementation of the 
        function a version marker in itself.
        */
        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hDllInst,
                          ("DllGetVersion"));
   
        if(pDllGetVersion) {    // DLL supports version retrieval function
            DLLVERSIONINFO    dvi;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
            HRESULT hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr)) { // Finally, the version is at our hands
                dwMajor = dvi.dwMajorVersion;
                dwMinor = dvi.dwMinorVersion;
                dwBuildNumber = dvi.dwBuildNumber;
            } else
                bRes = FALSE;   // Failure
        } else  // GetProcAddress failed, the DLL cannot tell its version
            bRes = FALSE;       // Failure

        FreeLibrary(hDllInst);  // Release DLL
    } else  
        bRes = FALSE;   // DLL could not be loaded

    if (!bRes) // Cannot read DLL version the nice way
	{
		for(int iDir = WIN_DIR; iDir <= NO_DIR; iDir++) // loop for each possible directory
		{
			StringCchCopy (szFileName,sizeof(szFileName)/sizeof(szFileName[0]), szDLLFileName);    // Save a file name copy for the loading
			bRes = CheckFileVersion (szFileName, iDir, 
                                 dwMajor, dwMinor, dwBuildNumber); // Try the ugly way
			if(bRes)
				break;
		};
		return bRes;
	}
    else
        return TRUE;
}

/***************************************************************************

   Function:   CheckFileVersion

   Purpose:    Check the version information of a given file

   Input:      File name
               File location (Windows dir, System dir, Current dir or none)
               Reference to Major number 
               Reference to Minor number 
               Reference to Build number 

   Output:     TRUE only if successful

   Remarks:    Trashes original file name

****************************************************************************/
BOOL CDLLVersion::CheckFileVersion (LPTSTR szFileName, int FileLoc, 
                                    DWORD &dwMajor, DWORD &dwMinor, 
                                    DWORD &dwBuildNumber)
{
LPTSTR   lpVersion;			            // String pointer to 'version' text
//UINT    uVersionLen;
DWORD   dwVerHnd=0;			            // An 'ignored' parameter, always '0'
//VS_FIXEDFILEINFO vsFileInfo;

//    FixFilePath (szFileName, FileLoc);  // Add necessary path prefix to file name

    DWORD dwVerInfoSize = GetFileVersionInfoSize (szFileName, &dwVerHnd);
    if (!dwVerInfoSize)     // Cannot reach the DLL file
        return FALSE;

    LPSTR lpstrVffInfo = 
           (LPSTR) malloc (dwVerInfoSize);  // Alloc memory for file info
    if (lpstrVffInfo == NULL)
        return FALSE;   // Allocation failed

        // Try to get the info
    if (!GetFileVersionInfo(szFileName, dwVerHnd, dwVerInfoSize, lpstrVffInfo)) {
        free (lpstrVffInfo);
        return FALSE;   // Cannot read the file information - 
                        // wierd, since we could read the information size
    }
		/* The below 'hex' value looks a little confusing, but
		   essentially what it is, is the hexidecimal representation
		   of a couple different values that represent the language
		   and character set that we are wanting string values for.
		   040904E4 is a very common one, because it means:
			 US English, Windows MultiLingual characterset
		   Or to pull it all apart:
		   04------        = SUBLANG_ENGLISH_USA
		   --09----        = LANG_ENGLISH
		   ----04E4 = 1252 = Codepage for Windows:Multilingual
        */
	/*static char fileVersion[256];
	LPVOID version=NULL;
	DWORD vLen,langD;
	BOOL retVal;

	sprintf(fileVersion,"\\VarFileInfo\\Translation");
	retVal = VerQueryValue (    lpstrVffInfo,  
                fileVersion, &version, (UINT *)&uVersionLen);
	if (retVal && vLen==4)
	{
			memcpy(&langD,version,4);
			sprintf(fileVersion,"\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion",(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, (langD & 0xff0000)>>16);			
	}
	else 
		sprintf(fileVersion,"\\StringFileInfo\\%04X04B0\\FileVersion",GetUserDefaultLangID());

	if (!VerQueryValue (    lpstrVffInfo, fileVersion, 
                (LPVOID *)&lpVersion, (UINT *)&uVersionLen))
		{	
		free (lpstrVffInfo);
		return FALSE;     // Query was unsuccessful	
		}
	*/
    // Now we have a string that looks like this :
    // "MajorVersion.MinorVersion.BuildNumber", so let's parse it
	lpVersion = getVersion(szFileName);
	m_stFullVersion = getVersion(szFileName);
    BOOL bRes = ParseVersionString (lpVersion, dwMajor, dwMinor, dwBuildNumber);
    if(!bRes)
		// Lets try for commas
		bRes = ParseVersionString1 (lpVersion, dwMajor, dwMinor, dwBuildNumber);
	free (lpstrVffInfo);
    return bRes;
}

/***************************************************************************

   Function:   ParseVersionString

   Purpose:    Parse version information string into 3 different numbers

   Input:      The version string formatted as "MajorVersion.MinorVersion.BuildNumber"
               Reference to Major number 
               Reference to Minor number 
               Reference to Build number 

   Output:     TRUE only if successful

   Remarks:    

****************************************************************************/
BOOL CDLLVersion::ParseVersionString (LPTSTR lpVersion, 
                                      DWORD &dwMajor, DWORD &dwMinor, 
                                      DWORD &dwBuildNumber)
{
        // Get first token (Major version number)
	if(lpVersion==NULL)
		return FALSE;
    LPTSTR token = _tcstok( lpVersion, TEXT(".") );  
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwMajor = _ttoi (token);

    token = _tcstok (NULL, TEXT("."));  // Get second token (Minor version number)
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwMinor = _ttoi (token);

    token = _tcstok (NULL, TEXT("."));  // Get third token (Build number)
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwBuildNumber = _ttoi (token);

    return TRUE;
}


/***************************************************************************

   Function:   FixFilePath

   Purpose:    Adds the correct path string to a file name according 
               to given file location 

   Input:      Original file name
               File location (Windows dir, System dir, Current dir or none)

   Output:     TRUE only if successful

   Remarks:    Trashes original file name

****************************************************************************/
BOOL CDLLVersion::FixFilePath (TCHAR * szFileName, int FileLoc)
{
    TCHAR    szPathStr [_MAX_PATH];      // Holds path prefix
   
    switch (FileLoc) {
        case WIN_DIR:
                // Get the name of the windows directory
            if (GetWindowsDirectory (szPathStr, _MAX_PATH) ==  0)  
                return FALSE;   // Cannot get windows directory
            break;

        case SYS_DIR:
                // Get the name of the windows SYSTEM directory
            if (GetSystemDirectory (szPathStr, _MAX_PATH) ==  0)  
                return FALSE;   // Cannot get system directory
            break;

        case CUR_DIR:
                // Get the name of the current directory
            if (GetCurrentDirectory (_MAX_PATH, szPathStr) ==  0)  
                return FALSE;   // Cannot get current directory
            break;

        case NO_DIR:
            StringCchCopy (szPathStr,sizeof(szPathStr)/sizeof(szPathStr[0]),_T(""));
            break;

        default:
            return FALSE;
    }
    StringCchCat(szPathStr,ARRAY_SIZE(szPathStr), _T("\\"));
    StringCchCat(szPathStr,ARRAY_SIZE(szPathStr), szFileName);
	StringCchCopy(szFileName, ARRAY_SIZE(szFileName),szPathStr);
    return TRUE;
}            



/***************************************************************************

   Function:   ParseVersionString

   Purpose:    Parse version information string into 3 different numbers

   Input:      The version string formatted as "MajorVersion.MinorVersion.BuildNumber"
               Reference to Major number 
               Reference to Minor number 
               Reference to Build number 

   Output:     TRUE only if successful

   Remarks:    

****************************************************************************/
BOOL CDLLVersion::ParseVersionString1 (LPTSTR lpVersion, 
                                      DWORD &dwMajor, DWORD &dwMinor, 
                                      DWORD &dwBuildNumber)
{
        // Get first token (Major version number)
	if(lpVersion==NULL)
		return FALSE;
    LPTSTR token = _tcstok( lpVersion, TEXT (",") );  
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwMajor = _ttoi (token);

    token = _tcstok (NULL, TEXT (","));  // Get second token (Minor version number)
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwMinor = _ttoi (token);

    token = _tcstok (NULL, TEXT (","));  // Get third token (Build number)
    if (token == NULL)  // End of string
        return FALSE;       // String ended prematurely
    dwBuildNumber = _ttoi (token);

    return TRUE;
}



// a static buffer is used to hold the version, calling this function
// a second time will erase previous information.
// long paths may be used for this function.
TCHAR *CDLLVersion::getVersion(TCHAR *fileName) {
	DWORD vSize;
	DWORD vLen,langD;
	BOOL retVal;	
	
	LPVOID version=NULL;
	LPVOID versionInfo=NULL;
	static TCHAR fileVersion[256];
	bool success = true;
	vSize = GetFileVersionInfoSize(fileName,&vLen);
	if (vSize) {
			versionInfo = malloc(vSize+1);
			if (GetFileVersionInfo(fileName,vLen,vSize,versionInfo))
			{			
			StringCchPrintf(fileVersion,sizeof(fileVersion)/sizeof(fileVersion[0]),_T("\\VarFileInfo\\Translation"));
			retVal = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen);						
			if (retVal && vLen==4) {
			memcpy(&langD,version,4);			
			StringCchPrintf(fileVersion,sizeof(fileVersion)/sizeof(fileVersion[0]),_T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, (langD & 0xff0000)>>16);			
			}
			else StringCchPrintf(fileVersion,sizeof(fileVersion)/sizeof(fileVersion[0]),_T("\\StringFileInfo\\%04X04B0\\FileVersion"),GetUserDefaultLangID());
			retVal = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen);
			if (!retVal) success = false;
			}
			else success = false;
		}
	else success=false;	
	
	if (success) {
		if (vLen<256)
			StringCchCopy(fileVersion,sizeof(fileVersion)/sizeof(fileVersion[0]),(TCHAR *)version);
		else {
			_tcsncpy(fileVersion,(TCHAR *)version,250);
			fileVersion[250]=0;			
		}
		if (versionInfo) free(versionInfo);
		versionInfo = NULL;
		return fileVersion;
	}
	else {
		if (versionInfo) free(versionInfo);
		versionInfo = NULL;
		return NULL;	
	}
}

LPCTSTR CDLLVersion::ToString()
{
	m_tcTextBuf[0]=0;
	if(IsValid())
	{
	//	wsprintf(m_tcTextBuf,")
	}
	return NULL;
}
