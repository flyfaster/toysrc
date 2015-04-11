/////////////////////////////////////////////////////////////////////////////
// DTempl.cpp : implementation of the CDlgItemTempl and CDlgTempl classes
//
// Written by Sridhar Madhugiri
// of Microsoft Product Support Services, Languages Developer Support
// Copyright (c) 1996 Microsoft Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if _MFC_VER < 0x0400
#include <winuser.h>
#else
#include <winnls.h>
#endif

#include "DTempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static char THIS__FILE[] = __FILE__;
/////////////////////////////////////////////////////////////////////////////
// CDlgItemTempl

// Following code is for setting up the runtime information
// Normally IMPLEMENT_SERIAL would handle this for us

CObject* __stdcall CDlgTempl::CDlgItemTempl::CreateObject() 
{ 
	return new CDlgTempl::CDlgItemTempl; 
} 

CRuntimeClass* __stdcall CDlgTempl::CDlgItemTempl::_GetBaseClass() 
{ 
	return (CRuntimeClass*) (&CObject::classCObject); 
}  

CRuntimeClass CDlgTempl::CDlgItemTempl::classCDlgItemTempl = 
{ 
	"CDlgTempl::CDlgItemTempl", 
	sizeof(class CDlgTempl::CDlgItemTempl), 
	1,
	CDlgTempl::CDlgItemTempl::CreateObject, 
#ifdef _AFXDLL
	&CDlgTempl::CDlgItemTempl::_GetBaseClass, 
#else
	(CRuntimeClass*)&CObject::classCObject,
#endif //!_AFXDLL
	0 
}; 

static const AFX_CLASSINIT _init_CDlgItemTempl(&CDlgTempl::CDlgItemTempl::classCDlgItemTempl); 
CRuntimeClass* CDlgTempl::CDlgItemTempl::GetRuntimeClass() const 
{ 
	return &CDlgTempl::CDlgItemTempl::classCDlgItemTempl; 
} 

CArchive& __stdcall operator>>(CArchive& ar, CDlgTempl::CDlgItemTempl* &pOb) 
{ 
	pOb = (CDlgTempl::CDlgItemTempl*) ar.ReadObject((&CDlgTempl::CDlgItemTempl::classCDlgItemTempl)); 
	return ar; 
}

//IMPLEMENT_SERIAL(CDlgTempl::CDlgItemTempl, CObject)


CDlgTempl::CDlgItemTempl::CDlgItemTempl() : 
    m_pControlClass(NULL), 
    m_pCaption(NULL),
    m_pInitData(NULL),
    m_nControlClassLength(0),
    m_nCaptionLength(0),
    m_nInitDataLength(0)
{
    memset(&m_dlgItemTemplate, 0, sizeof(m_dlgItemTemplate));
}

CDlgTempl::CDlgItemTempl::~CDlgItemTempl()
{
    Reset();
}

// CreateControl
//		Creates a control with the given properties
//
// There are 4 different combinations of data.
//

BOOL CDlgTempl::CDlgItemTempl::CreateControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
   	WORD controlClass, const TCHAR* szCaption, WORD nInitDataLength,
   	WORD* pInitData, DWORD dwExtendedStyle)
{
    // Set the DLGITEMTEMPLATE structure
    SetDlgItemTemplate(x, y, cx, cy, id, style, dwExtendedStyle);

    // Set the control class information
    if (!SetControlClassAtom(controlClass))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set the caption for the control
    if (!SetCaption(szCaption))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set extra initialization data
    if (nInitDataLength && !SetInitData(nInitDataLength, pInitData))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    return TRUE;
}

BOOL CDlgTempl::CDlgItemTempl::CreateControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
   	const TCHAR* controlClass, const TCHAR* szCaption, WORD nInitDataLength,
   	WORD* pInitData, DWORD dwExtendedStyle)
{
    // Set the DLGITEMTEMPLATE structure
    SetDlgItemTemplate(x, y, cx, cy, id, style, dwExtendedStyle);

    // Set the control class information
   	if (!SetControlClass(controlClass))
	{
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set the caption for the control
    if (!SetCaption(szCaption))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set extra initialization data
    if (nInitDataLength && !SetInitData(nInitDataLength, pInitData))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    return TRUE;
}

BOOL CDlgTempl::CDlgItemTempl::CreateControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
   	const TCHAR* controlClass, WORD wResourceID, WORD nInitDataLength,
   	WORD* pInitData, DWORD dwExtendedStyle)
{
    // Set the DLGITEMTEMPLATE structure
    SetDlgItemTemplate(x, y, cx, cy, id, style, dwExtendedStyle);

    // Set the control class information
   	if (!SetControlClass(controlClass))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set the caption for the control
    if (!SetCaptionID(wResourceID))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set extra initialization data
    if (nInitDataLength && !SetInitData(nInitDataLength, pInitData))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    return TRUE;
}

BOOL CDlgTempl::CDlgItemTempl::CreateControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
   	WORD controlClass, WORD wResourceID, WORD nInitDataLength,
   	WORD* pInitData, DWORD dwExtendedStyle)
{
    // Set the DLGITEMTEMPLATE structure
    SetDlgItemTemplate(x, y, cx, cy, id, style, dwExtendedStyle);

    // Set the control class information
    if (!SetControlClassAtom(controlClass))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set the caption for the control
    if (!SetCaptionID(wResourceID))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    // Set extra initialization data
    if (nInitDataLength && !SetInitData(nInitDataLength, pInitData))
    {
        Reset();
        TRACE(_T("Unable to Create Control\n"));
        return FALSE;
    }

    return TRUE;
}

// SetDlgItemTemplate
//		Sets the fixed part of the DLGITEMTEMPLATE structure
//
// Set the values in the fixed part of the template

void CDlgTempl::CDlgItemTempl::SetDlgItemTemplate(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style, 
                                          DWORD dwExtendedStyle)
{
    m_dlgItemTemplate.x                 = x;
    m_dlgItemTemplate.y                 = y;
    m_dlgItemTemplate.cx                = cx;
    m_dlgItemTemplate.cy                = cy;
    m_dlgItemTemplate.style             = style;
    m_dlgItemTemplate.dwExtendedStyle   = dwExtendedStyle;
    m_dlgItemTemplate.id                = id;
}

// Following 3 functions set the control class for the control
// Two distinct functions with the third overloaded for ANSI
// and UNICODE

BOOL CDlgTempl::CDlgItemTempl::SetControlClassAtom(WORD controlClass)
{
    ResetControlClass();

    if (!ValidControl(controlClass))
    {
        TRACE(_T("Invalid Control class\n"));
        return FALSE;
    }

    m_nControlClassLength = 2 * sizeof(WORD);
    m_pControlClass = new BYTE[m_nControlClassLength];
    if (!m_pControlClass)
    {
        m_nControlClassLength = 0;
        TRACE(_T("Unable to allocate Memory\n"));
        return FALSE;
    }

    WORD* p = (WORD*) m_pControlClass;
    *p++ = 0xFFFF;
    *p = controlClass;

    return TRUE;
}

BOOL CDlgTempl::CDlgItemTempl::SetControlClassA(const char* controlClass)
{
    ResetControlClass();

    UINT nControlClassLength = _mbstrlen(controlClass);
    WCHAR* pszControlClass = new WCHAR[nControlClassLength + 1];
        if (!pszControlClass)
    {
        m_nControlClassLength = 0;
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return FALSE;
    }

    mbstowcs(pszControlClass, controlClass, nControlClassLength + 1); 

    BOOL bRet = SetControlClassW(pszControlClass);

    delete []pszControlClass;

    return bRet;
}

BOOL CDlgTempl::CDlgItemTempl::SetControlClassW(const WCHAR* controlClass)
{
    ASSERT(controlClass);

    ResetControlClass();
    
    m_nControlClassLength = (wcslen(controlClass) + 1) * sizeof(WCHAR);
    m_pControlClass = new BYTE[m_nControlClassLength];
    if (!m_pControlClass)
    {
        m_nControlClassLength = 0;
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return FALSE;
    }

    wcscpy((WCHAR*)m_pControlClass, controlClass);

    return TRUE;
}

// Following 3 functions set the caption for the control
// Two distinct functions with the third overloaded for ANSI
// and UNICODE

BOOL CDlgTempl::CDlgItemTempl::SetCaptionID(WORD wCaptionID)
{
    ResetCaption();

    m_nCaptionLength = 2 * sizeof(WORD);
    m_pCaption = new BYTE[m_nCaptionLength];
    if (!m_pCaption)
    {
        m_nCaptionLength = 0;
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return FALSE;
    }

    WORD* p = (WORD*) m_pCaption;
    *p++ = 0xFFFF;
    *p = wCaptionID;

    return TRUE;
}

BOOL CDlgTempl::CDlgItemTempl::SetCaptionA(const char* szCaption)
{
    ResetCaption();

    UINT nCaptionLength = _mbstrlen(szCaption);
    WCHAR* pszCaption = new WCHAR[nCaptionLength + 1];
    if (!pszCaption)
    {
        m_nCaptionLength = 0;
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return FALSE;
    }
    mbstowcs(pszCaption, szCaption, nCaptionLength + 1); 

    BOOL bRet = SetCaptionW(pszCaption);

    delete []pszCaption;

    return bRet;
}

BOOL CDlgTempl::CDlgItemTempl::SetCaptionW(const WCHAR* szCaption)
{
    ResetCaption();

    m_nCaptionLength = (wcslen(szCaption) + 1) * sizeof(WCHAR);
    m_pCaption = new BYTE[m_nCaptionLength];
    if (!m_pCaption)
    {
        m_nCaptionLength = 0;
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return FALSE;
    }

    wcscpy((WCHAR*)m_pCaption, szCaption);

    return TRUE;
}

// Set the InitData part of the template for the control.
// The control should know how to use this data.

BOOL CDlgTempl::CDlgItemTempl::SetInitData(WORD nInitDataLength, const WORD* pInitData)
{
    ResetInitData();

    m_nInitDataLength = nInitDataLength;

    if (m_nInitDataLength)
    {
        m_pInitData = new BYTE[m_nInitDataLength];
        if (!m_pInitData)
        {
            m_nInitDataLength = 0;
            TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
            return FALSE;
        }

        WORD* p = (WORD*)m_pInitData;
        for ( ; nInitDataLength--; *p++ = *pInitData++);
    }

    return TRUE;
}

// Reset the template.

void CDlgTempl::CDlgItemTempl::Reset(void)
{
    memset(&m_dlgItemTemplate, 0, sizeof(m_dlgItemTemplate));
    ResetControlClass();
    ResetCaption();
    ResetInitData();
}

// Serialize the control to the archive.

void CDlgTempl::CDlgItemTempl::Serialize( CArchive& ar )
{
    if (ar.IsLoading())
    {
        Reset();
        ar.Read(&m_dlgItemTemplate, sizeof(m_dlgItemTemplate));

        ar >> m_nControlClassLength;
        m_pControlClass = new BYTE[m_nControlClassLength];
        ar.Read(m_pControlClass, m_nControlClassLength);

        ar >> m_nCaptionLength;
        m_pCaption = new BYTE[m_nCaptionLength];
        ar.Read(m_pCaption, m_nCaptionLength);

        ar >> m_nInitDataLength;
        if (m_nInitDataLength)
        {
            m_pCaption = new BYTE[m_nInitDataLength];
            ar.Read(m_pInitData, m_nInitDataLength);
        }
    }
    else
    {
        ar.Write(&m_dlgItemTemplate, sizeof(m_dlgItemTemplate));

        ar << m_nControlClassLength;
        ar.Write(m_pControlClass, m_nControlClassLength);

        ar << m_nCaptionLength;
        ar.Write(m_pCaption, m_nCaptionLength);

        ar << m_nInitDataLength;
        if (m_nInitDataLength)
        {
            ar.Write(m_pInitData, m_nInitDataLength);
        }
    }
}

// Create the template in memory at the location specified by 
// the pointer. This function creates the template in the
// required format. The memory pointed to by pMemory should
// be DWORD aligned and should be large enough to hold the 
// template for the control

void CDlgTempl::CDlgItemTempl::CreateControlInMemory(BYTE* pMemory)
{
    ASSERT(AlignDWord((ULONG)&pMemory) == (ULONG)&pMemory);

    ASSERT(AfxIsValidAddress(pMemory, GetLength()));

    BYTE* pMain = pMemory;

    // Copy constant part of the template
    DLGITEMTEMPLATE* pTemplate = (DLGITEMTEMPLATE*) pMain;
    *pTemplate = m_dlgItemTemplate;
    pMain = pMemory + sizeof(m_dlgItemTemplate);

    // Copy control class
    int nLenght = m_nControlClassLength;
    BYTE* p = (BYTE*) m_pControlClass;
    for (; nLenght--; *pMain++ = *p++);

    // Copy Caption
    nLenght = m_nCaptionLength;
    p = (BYTE*) m_pCaption;
    for (; nLenght--; *pMain++ = *p++);

    // Init Data length
    *((WORD *)pMain) = m_nInitDataLength;
    pMain += sizeof(WORD);

    // Copy Init Data
    nLenght = m_nInitDataLength;
    p = (BYTE*) m_pInitData;
    for (; nLenght--; *pMain++ = *p++);
}

// Parse the template in memory and fill the member variables.
// The control template starts at the location pointed by the 
// pointer. It has to point to a DWORD aligned boundary

BOOL CDlgTempl::CDlgItemTempl::CreateControlFromMemory(BYTE* p)
{
    ASSERT(AlignDWord((ULONG)&p) == (ULONG)&p);
    ASSERT(AfxIsValidAddress(p, GetLength(), FALSE));

    // Read in the constant part of the template
    m_dlgItemTemplate = *((DLGITEMTEMPLATE*) p);
    p += sizeof(DLGITEMTEMPLATE);

    // Read the control calss for the control
    if (*((WORD*) p) == 0xFFFF)
    {
        SetControlClassAtom(*(p + sizeof(WORD)));
    }
    else
    {
        SetControlClassW((WORD*) p);
    }
    p += m_nControlClassLength;

    // Read the control caption
    if (*((WORD*) p) == 0xFFFF)
    {
        SetCaptionID(*(p + sizeof(WORD)));
    }
    else
    {
        SetCaptionW((WORD*) p);
    }
    p += m_nCaptionLength;

    // Read the controls Init Data Length and the data
    SetInitData(*p, (WORD*) (p + sizeof(WORD)));

    return TRUE;
}


#ifdef _DEBUG
// Dumps the member variables for debugging
void CDlgTempl::CDlgItemTempl::Dump( CDumpContext& dc ) const
{
    CObject::Dump( dc );
    
    dc.HexDump(_T("m_dlgItemTemplate : "), (unsigned char *)&m_dlgItemTemplate, sizeof(m_dlgItemTemplate), 20);
    dc << "m_nControlClassLength : " << m_nControlClassLength << "\n";
  
    dc.HexDump(_T("m_pControlClass : "), m_pControlClass, m_nControlClassLength, 20);

    dc << _T("m_nCaptionLength : ") << m_nCaptionLength << "\n";
    dc.HexDump(_T("m_pCaption : "),m_pCaption, m_nCaptionLength, 20);

    dc << _T("m_nInitDataLength : ") << m_nInitDataLength << "\n";
    if (m_nInitDataLength)
    {
        dc.HexDump(_T("m_pInitData : "),m_pInitData, m_nInitDataLength, 20);
    }
}

#endif //_DEBUG

// DialogTempl.cpp
//


/////////////////////////////////////////////////////////////////////////////
// CDlgTempl
IMPLEMENT_SERIAL(CDlgTempl, CObject, 1)

CDlgTempl::CDlgTempl() : m_hDlgTemplate( NULL ), 
    m_lpDlgTemplate( NULL ), 
    m_nMenuNameLength(0),
    m_nClassNameLength(0),
    m_nCaptionLength(0),
    m_pMenuName(NULL),
    m_pClassName(NULL),
    m_pCaption(NULL),
    m_nPointSize(0),
	m_nFaceNameLength(0),
    m_pFaceName(NULL)
{
    memset(&m_dlgTemplate, 0, sizeof(m_dlgTemplate));
    SetMenuNameW(NULL);
    SetClassNameW(NULL);
    SetCaptionW(NULL);
}

CDlgTempl::~CDlgTempl()
{
    ResetAll(FALSE);
}

// Reset the members. If the argument is TRUE it puts out a 
// trace message before resetting the contents.

void CDlgTempl::ResetAll(BOOL bWarn /* = TRUE*/)
{
    if (bWarn)
    {
        TRACE(_T("Warning : Resetting Old Template\n"));
    }

    if (m_lpDlgTemplate) 
    {
        UnlockTemplate();
        m_lpDlgTemplate = NULL;
    }

    if (m_hDlgTemplate)
    {
		LocalFree(m_hDlgTemplate);
		m_hDlgTemplate = NULL;
	}

    if (m_pMenuName)
        delete []m_pMenuName;

    if (m_pClassName)
        delete []m_pClassName;

    if (m_pCaption)
        delete []m_pCaption;

    if (m_pFaceName)
        delete []m_pFaceName;


    for(POSITION pos = controlList.GetHeadPosition();
        pos != NULL;
        pos = controlList.GetHeadPosition())
    {
        CObject* p = controlList.GetAt(pos);
        controlList.RemoveAt(pos);
        delete p;
    }
}


// Serialize the template
void CDlgTempl::Serialize( CArchive& ar )
{
    if (ar.IsLoading())
    {
        ResetAll();
        ar.Read(&m_dlgTemplate, sizeof(m_dlgTemplate));

        ar >> m_nMenuNameLength;
        m_pMenuName = new BYTE[m_nMenuNameLength];
        if (!m_pMenuName)
        {
            ResetAll(FALSE);
            TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        }
        ar.Read(m_pMenuName,m_nMenuNameLength);

        ar >> m_nClassNameLength;
        m_pClassName = new BYTE[m_nClassNameLength];
        if (!m_pClassName)
        {
            ResetAll(FALSE);
            TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
            return;
        }

        ar.Read(m_pClassName,m_nClassNameLength);
        ar >> m_nCaptionLength;
        m_pCaption = new BYTE[m_nCaptionLength];
        if (!m_pCaption)
        {
            TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
            ResetAll(FALSE);
        }
        ar.Read(m_pCaption,m_nCaptionLength);

        if (m_dlgTemplate.style & DS_SETFONT)
        {
            ar >> m_nPointSize;
            ar >> m_nFaceNameLength;
            m_pFaceName = new BYTE[m_nFaceNameLength];
            if (!m_pFaceName)
            {
                ResetAll(FALSE);
                TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
                return;
            }
            ar.Read(m_pFaceName,m_nFaceNameLength);
        }
    }
    else
    {
        ar.Write(&m_dlgTemplate, sizeof(m_dlgTemplate));

        ar << m_nMenuNameLength;
        ar.Write(m_pMenuName,m_nMenuNameLength);

        ar << m_nClassNameLength;
        ar.Write(m_pClassName,m_nClassNameLength);

        ar << m_nCaptionLength;
        ar.Write(m_pCaption,m_nCaptionLength);

        if (m_dlgTemplate.style & DS_SETFONT)
        {
            ar << m_nPointSize;
            ar << m_nFaceNameLength;
            ar.Write(m_pFaceName,m_nFaceNameLength);
        }
    }

    controlList.Serialize(ar);
}


// set the constant part of the dialog template
BOOL CDlgTempl::SetDlgTemplate(DWORD style, 
    DWORD dwExtendedStyle, 
    short x, short y, 
    short cx, short cy)
{
    m_dlgTemplate.style = style;
    m_dlgTemplate.dwExtendedStyle = dwExtendedStyle;
    m_dlgTemplate.x = x;
    m_dlgTemplate.y = y;
    m_dlgTemplate.cx = cx;
    m_dlgTemplate.cy = cy;

    return TRUE;
}


// Returns the size of buffer required to create the template in memory
UINT CDlgTempl::GetLength()
{
    UINT nTemplateLength = GetHeaderLength();

    POSITION pos = controlList.GetHeadPosition();

    while(pos != NULL)
    {
        CDlgItemTempl* pControl = (CDlgItemTempl*)controlList.GetNext(pos);
        nTemplateLength += pControl->GetLength();
    }

    return nTemplateLength;
}

// Set the menu for the dialog.
// three overloaded members to specify the menu
BOOL CDlgTempl::SetMenuID(WORD wResourceID)
{
    if (!wResourceID)
    {
        return SetMenuNameW(NULL);
    }

    if (m_pMenuName)
    {
        delete []m_pMenuName;
    }

    m_nMenuNameLength = 2 * sizeof(WORD);
    m_pMenuName = new BYTE[m_nMenuNameLength];
    if (!m_pMenuName)
    {
        m_nMenuNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    WORD* p = (WORD*) m_pMenuName;
    *p++ = 0xFFFF;
    *p = wResourceID;

    return TRUE;
}

BOOL CDlgTempl::SetMenuNameA(const char* szMenuName)
{
    BOOL bRet;

    if (m_pMenuName)
    {
        delete []m_pMenuName;
        m_pMenuName = NULL;
        m_nMenuNameLength = 0;
    }

    if (szMenuName)
    {
        UINT nMenuNameLength = _mbstrlen(szMenuName);
        WCHAR* pszMenuName = new WCHAR[nMenuNameLength + 1];
        if (!pszMenuName)
        {
            m_nMenuNameLength = 0;
            AfxThrowMemoryException();
            return FALSE;
        }
        mbstowcs(pszMenuName, szMenuName, nMenuNameLength + 1); 
        bRet = SetMenuNameW(pszMenuName);

        delete []pszMenuName;
    }
    else
    {
        bRet = SetMenuNameW(NULL);
    }
    return bRet;
}

BOOL CDlgTempl::SetMenuNameW(const WCHAR* szMenuName)
{
    if (m_pMenuName)
    {
        delete []m_pMenuName;
    }

    m_nMenuNameLength = sizeof(WORD);
    if (szMenuName)
    {
        m_nMenuNameLength += (wcslen(szMenuName)) * sizeof(WCHAR);
    }
    
    m_pMenuName = new BYTE[m_nMenuNameLength];
    if (!m_pMenuName)
    {
        m_nMenuNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    *((WORD*)m_pMenuName) = 0x0000;
    if (szMenuName)
    {
        wcscpy((WCHAR*)m_pMenuName, szMenuName);
    }
    
    return TRUE;
}

// Set the window class for the dialog.
// three overloaded members to specify the window class
// if the window class is 0(default) then the standard class 
// is used.

BOOL CDlgTempl::SetClassAtom(WORD wClassAtom)
{
    if (m_pClassName)
    {
        delete []m_pClassName;
    }

    m_nClassNameLength = 2 * sizeof(WORD);

    m_pClassName = new BYTE[m_nClassNameLength];

    if (!m_pClassName)
    {
        m_nClassNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    *((WORD*) m_pClassName) = 0xFFFF;
    *((WORD*) m_pClassName + sizeof(WORD)) = wClassAtom;

    return TRUE;
}

BOOL CDlgTempl::SetClassNameA(const char* szClassName)
{
    if (m_pClassName)
    {
        delete []m_pClassName;
        m_pClassName = NULL;
        m_nClassNameLength = 0;
    }

    if (szClassName)
    {
        UINT nClassNameLength = _mbstrlen(szClassName);
        WCHAR* pszClassName = new WCHAR[nClassNameLength + 1];
        if (!pszClassName)
        {
            m_nClassNameLength = 0;
            AfxThrowMemoryException();
            return FALSE;
        }
        mbstowcs(pszClassName, szClassName, nClassNameLength + 1); 

        BOOL bRet = SetClassNameW(pszClassName);

        delete []pszClassName;

        return bRet;
    }
    else
    {
        return SetClassNameW(NULL);
    }
}

BOOL CDlgTempl::SetClassNameW(const WCHAR* szClassName)
{
    if (m_pClassName)
    {
        delete []m_pClassName;
    }

    m_nClassNameLength = sizeof(WORD);
    if (szClassName)
    {
        m_nClassNameLength += (wcslen(szClassName)) * sizeof(WCHAR);
    }

    m_pClassName = new BYTE[m_nClassNameLength];
    if (!m_pClassName)
    {
        m_nClassNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    *((WORD*) m_pClassName) = 0x0000;
    if (szClassName)
    {
        wcscpy((WCHAR*)m_pClassName, szClassName);
    }

    return TRUE;
}

BOOL CDlgTempl::SetCaptionA(const char* szCaption)
{
    if (m_pCaption)
    {
        delete []m_pCaption;
        m_pCaption = NULL;
    }

    if (szCaption)
    {
        UINT nCaptionLength = _mbstrlen(szCaption);
        WCHAR* pszCaption = new WCHAR[nCaptionLength + 1];
        if (!pszCaption)
        {
            nCaptionLength = 0;
            AfxThrowMemoryException();
            return FALSE;
        }

        mbstowcs(pszCaption, szCaption, nCaptionLength + 1); 

        BOOL bRet = SetCaptionW(pszCaption);

        delete []pszCaption;

        return bRet;
    }
    else
    {
        return SetCaptionW(NULL);
    }
}

BOOL CDlgTempl::SetCaptionW(const WCHAR* szCaption)
{
    if (m_pCaption)
    {
        delete []m_pCaption;
    }
        
    m_nCaptionLength = sizeof(WCHAR);

    if (szCaption)
    {
        m_nCaptionLength += (wcslen(szCaption)) * sizeof(WCHAR);
    }
    
    m_pCaption = new BYTE[m_nCaptionLength];
    if (!m_pCaption)
    {
        m_nCaptionLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    *((WORD*) m_pCaption) = 0x0000;
    if (szCaption)
    {
        wcscpy((WCHAR*)m_pCaption, szCaption);
    }

    return TRUE;
}

// Set the font for the dialog
// Two overloaded functions to handle ANSI/UNICODE

BOOL CDlgTempl::SetFontInfoA(WORD wPointSize, const char* szFaceName)
{
    if (m_pFaceName)
    {
        delete []m_pFaceName;
        m_pFaceName = NULL;
        m_dlgTemplate.style &= ~DS_SETFONT;
        m_nPointSize = 0;
    }

    if (!szFaceName)
    {
        TRACE (_T("Warning : No Font Specified\n"));
        return TRUE;
    }

    UINT nFaceNameLength = _mbstrlen(szFaceName);
    WCHAR* pszFaceName = new WCHAR[nFaceNameLength + 1];
    if (!pszFaceName)
    {
        m_nFaceNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    mbstowcs(pszFaceName, szFaceName, nFaceNameLength + 1); 

    BOOL bRet = SetFontInfoW(wPointSize, pszFaceName);

    delete []pszFaceName;

    return bRet;
}

BOOL CDlgTempl::SetFontInfoW(WORD wPointSize, const WCHAR* szFaceName)
{
    if (m_pFaceName)
    {
        delete []m_pFaceName;
        m_dlgTemplate.style &= ~DS_SETFONT;
        m_nPointSize = 0;
    }

    if (!szFaceName)
    {
        TRACE (_T("Warning : No Font Specified\n"));
        return TRUE;
    }

    m_nFaceNameLength = (wcslen(szFaceName) + 1) * sizeof(WCHAR);
    m_pFaceName = new BYTE[m_nFaceNameLength];
    if (!m_pFaceName)
    {
        m_nFaceNameLength = 0;
        AfxThrowMemoryException();
        return FALSE;
    }

    wcscpy((WCHAR*)m_pFaceName, szFaceName);

    m_dlgTemplate.style |= DS_SETFONT;
    m_nPointSize = wPointSize;


    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////  
//
//	CDlgTempl::AddControl
//    
// 		Adds a control specified by the input parameters to the dialog 
//		template being constructed
//                        
//	Parameters :
//	------------
//
//	x			- Horizontal position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	y			- Vertical position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	cx			- Width of the control window in dialog units
//
//	cy			- Height of the control window in dialog units
//
//	id			- ID of the control
//
//	style		- Control window styles
//
//	controlClass	- Identifies one of the following predefined window 
//					  classes:
//						0x0080	Button
//						0x0081	Edit
//						0x0082	Static
//						0x0083	List box
//						0x0084	Scroll bar
//						0x0085	Combo box
//
//	szCaption	- Control window’s title string in UNICODE
//
//	nInitDataLength	- number of control specific data bytes
//
//	pInitData		- Array of bytes that is control-specific data
//
//  dwExtendedStyle - Extended styles for the control
//
//
//	Returns :   
//	---------
//
//	True 		- Control was sucessfully added to the template
//	FALSE		- Control could not be added to the template
//
//////////////////////////////////////////////////////////////////////////////


BOOL CDlgTempl::AddControl(
	WORD  x,
    WORD  y,
    WORD  cx,
    WORD  cy,
    WORD  id,
    DWORD style,
    WORD controlClass,
    const TCHAR* szCaption,
	WORD  nInitDataLength,	//in BYTES
    WORD* pInitData,
    DWORD dwExtendedStyle)
{
    CDlgItemTempl* pDialogItemTempl = new CDlgItemTempl;

    if (pDialogItemTempl && 
        pDialogItemTempl->CreateControl(x, y, cx, cy, id, style,
            controlClass, szCaption, nInitDataLength, pInitData, dwExtendedStyle))
    {
        controlList.AddTail(pDialogItemTempl);
        m_dlgTemplate.cdit ++;
        return TRUE;
    }
    if (pDialogItemTempl)
        delete pDialogItemTempl;

    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////  
//
//	CDlgTempl::AddControl
//    
// 		Adds a control specified by the input parameters to the dialog 
//		template being constructed
//                        
//	Parameters :
//	------------
//
//	x			- Horizontal position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	y			- Vertical position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	cx			- Width of the control window in dialog units
//
//	cy			- Height of the control window in dialog units
//
//	id			- ID of the control
//
//	style		- Control window styles
//
//	controlClass	- UNICODE string identifying the class name of the control
//					  window
//
//	szCaption	- Control window’s title string in UNICDOE
//
//	nInitDataLength	- number of control specific data bytes
//
//	pInitData		- Array of bytes that is control-specific data
//
//  dwExtendedStyle - Extended styles for the control
//
//
//	Returns :   
//	---------
//
//	True 		- Control was sucessfully added to the template
//	FALSE		- Control could not be added to the template
//
//////////////////////////////////////////////////////////////////////////////

BOOL CDlgTempl::AddControl(
	WORD  x,
    WORD  y,
    WORD  cx,
    WORD  cy,
    WORD  id,
    DWORD style,
    const TCHAR* controlClass,
    const TCHAR* szCaption,
	WORD  nInitDataLength,	//in BYTES
    WORD* pInitData,
    DWORD dwExtendedStyle)
{
    CDlgItemTempl* pDialogItemTempl = new CDlgItemTempl;

    if (pDialogItemTempl && 
        pDialogItemTempl->CreateControl(x, y, cx, cy, id, style,
            controlClass, szCaption, nInitDataLength, pInitData, dwExtendedStyle))
    {
        controlList.AddTail(pDialogItemTempl);
        m_dlgTemplate.cdit ++;
        return TRUE;
    }
    if (pDialogItemTempl)
        delete pDialogItemTempl;

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////  
//
//	CDlgTempl::AddControl
//    
// 		Adds a control specified by the input parameters to the dialog 
//		template being constructed
//                        
//	Parameters :
//	------------
//
//	x			- Horizontal position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	y			- Vertical position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	cx			- Width of the control window in dialog units
//
//	cy			- Height of the control window in dialog units
//
//	id			- ID of the control
//
//	style		- Control window styles
//
//	controlClass	- UNICODE string identifying the class name of the control
//					  window
//
//	wResourceID	- Ordinal number identifying a resource such as an icon for
//				  static controls with SS_ICON style
//
//	nInitDataLength	- number of control specific data bytes
//
//	pInitData		- Array of bytes that is control-specific data
//
//  dwExtendedStyle - Extended styles for the control
//
//
//	Returns :   
//	---------
//
//	True 		- Control was sucessfully added to the template
//	FALSE		- Control could not be added to the template
//
//////////////////////////////////////////////////////////////////////////////

BOOL CDlgTempl::AddControl(
	WORD  x,
    WORD  y,
    WORD  cx,
    WORD  cy,
    WORD  id,
    DWORD style,
    const TCHAR* controlClass,
    WORD wResourceID,
	WORD  nInitDataLength,	//in BYTES
    WORD* pInitData,
    DWORD dwExtendedStyle)
{
    CDlgItemTempl* pDialogItemTempl = new CDlgItemTempl;

    if (pDialogItemTempl && 
        pDialogItemTempl->CreateControl(x, y, cx, cy, id, style,
            controlClass, wResourceID, nInitDataLength, pInitData, dwExtendedStyle))
    {
        controlList.AddTail(pDialogItemTempl);
        m_dlgTemplate.cdit ++;
        return TRUE;
    }
    if (pDialogItemTempl)
        delete pDialogItemTempl;

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////  
//
//	CDlgTempl::AddControl
//    
// 		Adds a control specified by the input parameters to the dialog 
//		template being constructed
//                        
//	Parameters :
//	------------
//
//	x			- Horizontal position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	y			- Vertical position of the control window in dialog units,
//				  relative to upper-left corner of dialog box’s client area
//
//	cx			- Width of the control window in dialog units
//
//	cy			- Height of the control window in dialog units
//
//	id			- ID of the control
//
//	style		- Control window styles
//
//	controlClass	- Identifies one of the following predefined window 
//					  classes:
//						0x0080	Button
//						0x0081	Edit
//						0x0082	Static
//						0x0083	List box
//						0x0084	Scroll bar
//						0x0085	Combo box
//
//	wResourceID	- Ordinal number identifying a resource such as an icon for
//				  static controls with SS_ICON style
//
//	nInitDataLength	- number of control specific data bytes
//
//	pInitData		- Array of bytes that is control-specific data
//
//  dwExtendedStyle - Extended styles for the control
//
//
//	Returns :   
//	---------
//
//	True 		- Control was sucessfully added to the template
//	FALSE		- Control could not be added to the template
//
//////////////////////////////////////////////////////////////////////////////

BOOL CDlgTempl::AddControl(
	WORD  x,
    WORD  y,
    WORD  cx,
    WORD  cy,
    WORD  id,
    DWORD style,
    WORD controlClass,
    WORD wResourceID,
	WORD  nInitDataLength,	//in BYTES
    WORD* pInitData,
    DWORD dwExtendedStyle)
{
    CDlgItemTempl* pDialogItemTempl = new CDlgItemTempl;

    if (pDialogItemTempl && 
        pDialogItemTempl->CreateControl(x, y, cx, cy, id, style,
            controlClass, wResourceID, nInitDataLength, pInitData, dwExtendedStyle))
    {
        controlList.AddTail(pDialogItemTempl);
        m_dlgTemplate.cdit ++;
        return TRUE;
    }
    if (pDialogItemTempl)
        delete pDialogItemTempl;

    return FALSE;
}

// Call this function to allocate Local memory lock it and create the template in this
// memory. The returned pointer can be used to pass into CreateDialogIndirect.
// The memory should be unlocked using UnlockTemplate function
void* CDlgTempl::LockTemplate()
{
    if (m_lpDlgTemplate) 
    {
        TRACE(_T("Warning : This template has already been locked\n"));
        return NULL;
    }

    m_hDlgTemplate = LocalAlloc(LHND, GetLength());
    if (m_hDlgTemplate == NULL)
    {
        TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
        return NULL;
    }

    m_lpDlgTemplate = LocalLock(m_hDlgTemplate);

    if (m_lpDlgTemplate == NULL)
    {
        TRACE(_T("Failure : %s,%d - Unable To Lock Memory\n"), __FILE__, __LINE__);
        LocalFree(m_hDlgTemplate);
        m_hDlgTemplate = NULL;
        return NULL;
    }

    CreateTemplateInMemory(m_lpDlgTemplate);
    
    return m_lpDlgTemplate;
}

// Unlock a previously locked template and frees up the memory allocated by LockTemplate
void CDlgTempl::UnlockTemplate()
{
    if (m_lpDlgTemplate)
        LocalUnlock(m_hDlgTemplate);
    
    m_lpDlgTemplate = NULL;

    LocalFree(m_hDlgTemplate);
    m_hDlgTemplate = NULL;
}


// Create the template in memory using the data stored in the member variables.
// The memory should be large enough to hold the template.
BOOL CDlgTempl::CreateTemplateInMemory(void* pMemory)
{
    if (!pMemory || !AfxIsValidAddress(pMemory, GetLength()))
    {
        TRACE(_T("Failure : Invalid pointer\n"));
        return FALSE;
    }

    BYTE* pMain = (BYTE*) pMemory;
    DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)pMemory;

    *pTemplate = m_dlgTemplate;

    pMain += sizeof(m_dlgTemplate);

    int nLenght = m_nMenuNameLength;
    BYTE* p = (BYTE*) m_pMenuName;

    for (; nLenght--; *pMain++ = *p++);

    nLenght = m_nClassNameLength;
    p = (BYTE*) m_pClassName;

    for (; nLenght--; *pMain++ = *p++);

    nLenght = m_nCaptionLength;
    p = (BYTE*) m_pCaption;

    for (; nLenght--; *pMain++ = *p++);

    if (m_dlgTemplate.style & DS_SETFONT)
    {
        *((WORD *)pMain) = m_nPointSize;

        pMain += sizeof(WORD);
        
        nLenght = m_nFaceNameLength;
        p = (BYTE*) m_pFaceName;

        for (; nLenght--; *pMain++ = *p++);
    }

    pMain = (BYTE*) pMemory + GetHeaderLength();

    ASSERT(m_dlgTemplate.cdit == controlList.GetCount());

    POSITION pos = controlList.GetHeadPosition();

    while (pos != NULL)
    {
        CDlgItemTempl* pControl = (CDlgItemTempl*)controlList.GetNext(pos);

        pControl->CreateControlInMemory(pMain);

        pMain += pControl->GetLength();
    }

    return TRUE;
}

// Find the control in the template
//
// The control is identified as follows
// 
//      id      -   identifies the control to be moved and its meaning 
//                  is dependent on the nFlag parameter.
//
//                  Can be the control ID 
//                  or
//                  Present Z order
//
//      nFlag   -   specifies the meaning of id
//
//              Valid Values
//
//                  CDlgTempl::byItemID          - id specifies the control ID
//           
//                  CDlgTempl::byItemPosition    - id specifies the controls present 
//                                                    Z order
//

CDlgTempl::CDlgItemTempl* CDlgTempl::FindControl(UINT id, UINT nFlag)
{
    ASSERT(nFlag == byItemID || nFlag == byItemPosition);

    if (nFlag == byItemID && (id = FindControlByID(id)) == (UINT) -1)
    {
        TRACE(_T("Failure : %s,%d - Invalid item ID\n"), __FILE__, __LINE__);
        return NULL;
    }

    return FindControlByPosition(id);
}

UINT CDlgTempl::FindControlByID(UINT id)
{
    // cannot find static controls by their ID
    if (id == (UINT) -1)
    {
        TRACE(_T("Failure : %s,%d - Static controls cannot be located by theit ID\n"), __FILE__, __LINE__);
        return (UINT) -1;
    }

    POSITION pos = controlList.GetHeadPosition();
    UINT nIndex = 0;

    while (pos != NULL)
    {
        CDlgItemTempl* pControl = (CDlgItemTempl*)controlList.GetNext(pos);
        if (pControl->GetID() == id)
            return nIndex;
        nIndex++;
    }

    return (UINT) -1;
}

CDlgTempl::CDlgItemTempl* CDlgTempl::FindControlByPosition(UINT position)
{
    return (CDlgItemTempl*) controlList.GetAt(controlList.FindIndex(position));
}

// Remove the control from the template
//
// The control is identified as follows
// 
//      id      -   identifies the control to be moved and its meaning 
//                  is dependent on the nFlag parameter.
//
//                  Can be the control ID 
//                  or
//                  Present Z order
//
//      nFlag   -   specifies the meaning of id
//
//              Valid Values
//
//                  CDlgTempl::byItemID          - id specifies the control ID
//           
//                  CDlgTempl::byItemPosition    - id specifies the controls present 
//                                                    Z order
//
//      bDelete -   specifies whether the control which is of type CDlgItemTempl
//                  has to be deleted or returned
//          
//                  TRUE - Delete the control
//                  FALSE - return the deleted control in ppControl. It is the pointer
//                          is returned then it is not freed when the template is destroyed
//                          
//      ppControl - valid pointer CDlgItemTempl* which will point to the deleted control
//                  if bDelete is FALSE
//

BOOL CDlgTempl::RemoveControl(UINT id, UINT nFlag, BOOL bDelete, CDlgItemTempl** ppControl)
{
    ASSERT(nFlag == byItemID || nFlag == byItemPosition);

    if (nFlag == byItemID && (id = FindControlByID(id)) == (UINT) -1)
    {
        return FALSE;
    }

    if ((int)id >= controlList.GetCount())
    {
        TRACE(_T("Failure : %s,%d - Invalid item position\n"), __FILE__, __LINE__);
        return FALSE;
    }


    ASSERT(*ppControl);
    *ppControl = NULL;

    *ppControl = (CDlgItemTempl*) controlList.GetAt(controlList.FindIndex(id));

    controlList.RemoveAt(controlList.FindIndex(id));

    if (bDelete)
    {
        delete *ppControl;
        *ppControl = NULL;
    }

    return TRUE;
}


// Move the control in the Z order so that it has a Z order identified by nPosition
//
// The control is identified as follows
// 
//      id      -   identifies the control to be moved and its meaning 
//                  is dependent on the nFlag parameter.
//
//                  Can be the control ID 
//                  or
//                  Present Z order
//
//      nFlag   -   specifies the meaning of id
//
//              Valid Values
//
//                  CDlgTempl::byItemID          - id specifies the control ID
//           
//                  CDlgTempl::byItemPosition    - id specifies the controls present 
//                                                    Z order
//
//      nPosition - specifies the Z order position of the control.
//

BOOL CDlgTempl::MoveControl(UINT id, UINT nFlag, UINT nPosition)
{
    ASSERT(nFlag == byItemID || nFlag == byItemPosition);

    if (nFlag == byItemID && (id = FindControlByID(id)) == (UINT) -1)
    {
        return FALSE;
    }

    if ((int)id >= controlList.GetCount())
    {
        TRACE(_T("Failure : %s,%d - Invalid item position\n"), __FILE__, __LINE__);
        return FALSE;
    }

    CObject *pControl = controlList.GetAt(controlList.FindIndex(id));
    controlList.RemoveAt(controlList.FindIndex(id));

    if ((int)nPosition >= controlList.GetCount())
    {
        if(controlList.AddTail(pControl))
            return TRUE;
        else
            return FALSE;
    }

    controlList.InsertBefore(controlList.FindIndex(nPosition), pControl);
    return TRUE;
}


// Parses a resource template and initializes the members from teh template.
// Other functions can then be called to manipulate this function

BOOL CDlgTempl::FromResource(LPCTSTR szTemplateName)
{
    // The following function is an undocumented function and 
    // could break in future versions of MFC
	HINSTANCE hInst = AfxFindResourceHandle(szTemplateName, RT_DIALOG);
    if (!hInst)
        return FALSE;

	HRSRC hResource = ::FindResource(hInst, szTemplateName, RT_DIALOG);
    if (!hResource)
        return FALSE;

	HLOCAL hDialogTemplate = LoadResource(hInst, hResource);
    if (!hDialogTemplate)
        return FALSE;

    BYTE* pDialogTemplate = (BYTE *) LockResource(hDialogTemplate);

    if (!pDialogTemplate)
    {
        FreeResource(hDialogTemplate);
        return FALSE;
    }

    BOOL bRet = CreateTemplateFromMemory(pDialogTemplate);

	UnlockResource(hDialogTemplate);
	FreeResource(hDialogTemplate);

    return bRet;
}


// Parses the template in memory and sets the members and
// creates a list of controls

BOOL CDlgTempl::CreateTemplateFromMemory(BYTE* p)
{
    m_dlgTemplate = *((DLGTEMPLATE *) p);

    p += sizeof(DLGTEMPLATE);

    if (*((WORD *) p) == 0xFFFF)
    {
        SetMenuID(*(p + sizeof(WORD)));
    }
    else
    {
        SetMenuNameW((WCHAR*) p);
    }

    p += m_nMenuNameLength;

    if (*((WORD*) p) == 0xFFFF)
    {
        SetClassAtom(*(p + sizeof(WORD)));
    }
    else
    {
        SetClassNameW((WORD*)p);
    }

    p += m_nClassNameLength;

    SetCaptionW((WCHAR*) p);

    p += m_nCaptionLength;

    if (m_dlgTemplate.style & DS_SETFONT)
    {
        SetFontInfoW(*((WORD*) p), (WCHAR*)(p + sizeof(WORD)));
        p += sizeof(WORD) + m_nFaceNameLength;
    }

    p = (BYTE*)AlignDWord((UINT)p);

    for (int i = 0; i < m_dlgTemplate.cdit; i++)
    {
        CDlgItemTempl* pDialogItemTempl = new CDlgItemTempl;

        if (!pDialogItemTempl)
        {
            ResetAll(FALSE);
            TRACE(_T("Failure : %s,%d - Unable To Allocate Memory\n"), __FILE__, __LINE__);
            return FALSE;
        }

        if (pDialogItemTempl->CreateControlFromMemory(p))
        {
            p += pDialogItemTempl->GetLength();
            controlList.AddTail(pDialogItemTempl);
        }
        else
        {
            ResetAll();
            TRACE(_T("Failure : %s,%d - Invalid template\n"), __FILE__, __LINE__);
            return FALSE;
        }
    }
    return TRUE;
}


#ifdef _DEBUG

void CDlgTempl::Dump( CDumpContext& dc ) const
{
        dc.HexDump(_T("DilaogTemplate : "), (unsigned char*)&m_dlgTemplate, sizeof(m_dlgTemplate), 16);

        dc << _T("Menu : ") <<  m_nMenuNameLength << _T("\n");
        dc.HexDump(_T(""), (unsigned char*)m_pMenuName, m_nMenuNameLength, 16);

        dc << _T("Class : ") << m_nClassNameLength << _T("\n");
        dc.HexDump(_T(""), (unsigned char*)m_pClassName, m_nClassNameLength, 16);

        dc << _T("Caption : ") << m_nCaptionLength << _T("\n");
        dc.HexDump(_T(""), (unsigned char*)m_pCaption, m_nCaptionLength, 16);

        if (m_dlgTemplate.style & DS_SETFONT)
        {
            dc << _T("Font : ") <<  m_nPointSize << _T("\n") << m_nFaceNameLength << _T("\n");
            dc.HexDump(_T(""), (unsigned char*)m_pFaceName, m_nFaceNameLength, 16);
        }

        controlList.Dump(dc);
}

#endif
