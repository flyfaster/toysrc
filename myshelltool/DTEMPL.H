/////////////////////////////////////////////////////////////////////////////
// DTempl.h : interface of the CDlgItemTempl and CDlgTempl classes
//
// Written by Sridhar Madhugiri
// of Microsoft Product Support Services, Languages Developer Support
// Copyright (c) 1996 Microsoft Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

/*
typedef struct {
    DWORD style;
    DWORD dwExtendedStyle;
    WORD cdit;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATE; 

typedef DLGTEMPLATE *LPDLGTEMPLATEA;
typedef DLGTEMPLATE *LPDLGTEMPLATEW;


//	DLGITEMTEMPLATE defined in WinUser.H


typedef struct { 
    DWORD style; 
    DWORD dwExtendedStyle; 
    short x; 
    short y; 
    short cx; 
    short cy; 
    WORD  id; 
} DLGITEMTEMPLATE; 

typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEA;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEW;

*/

// These values can be used for specifying the
// standard controls. They can also be specified
// by their names passed as strings

#define CONTROL_BUTTON		0x0080	//Button
#define CONTROL_EDIT		0x0081	//Edit
#define CONTROL_STATIC		0x0082	//Static
#define CONTROL_LISTBOX		0x0083	//List box
#define CONTROL_SCROLLBAR	0x0084	//Scroll bar
#define CONTROL_COMBOBOX 	0x0085	//Combo box

#define CONTROL_CLASS_FIRST	CONTROL_BUTTON
#define CONTROL_CLASS_LAST	CONTROL_COMBOBOX

/////////////////////////////////////////////////////////////////////////////

// Map functions to UNICODE or ANSI depending on the build.
// the functions with out the W or A suffix can be used and
// type of string passed in depends on the build
// The W or A function can also be called as long as the
// correct type of string is passed in.

#ifdef UNICODE

#define SetCaption      SetCaptionW
#define SetControlClass SetControlClassW

#else

#define SetCaption      SetCaptionA
#define SetControlClass SetControlClassA

#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTempl
class CDlgTempl : public CObject
{
public:
	class CDlgItemTempl : public CObject
	{
	// Construction
	public:
		inline void ResetControlClass(void)
		{
			if (m_pControlClass)
			   delete []m_pControlClass;

			m_pControlClass         = NULL;
			m_nControlClassLength   = 0;
		}

		inline void ResetCaption(void)
		{
			if (m_pCaption)
			   delete []m_pCaption;

			m_pCaption          = NULL;
			m_nCaptionLength    = 0;
		}

		inline void ResetInitData(void)
		{
			if (m_pInitData)
				delete []m_pInitData;

			m_pInitData = NULL;
			m_nInitDataLength = 0;
		}

		void Reset(void);

		void SetDlgItemTemplate(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style, DWORD dwExtendedStyle);
		
		CDlgItemTempl();

		DWORD GetStyle() { return m_dlgItemTemplate.style; }

		DWORD GetExtendedStyle() { return m_dlgItemTemplate.dwExtendedStyle; }

		CPoint GetPosition() { return CPoint(m_dlgItemTemplate.x, m_dlgItemTemplate.y); }

		CSize GetSize() { return CSize(m_dlgItemTemplate.cx, m_dlgItemTemplate.cy); }

		WORD GetID() { return m_dlgItemTemplate.id; }

		DWORD SetStyle(DWORD style) 
		{
			DWORD oldStyle = m_dlgItemTemplate.style;
			m_dlgItemTemplate.style = style;
			return oldStyle;
		}

		DWORD SetExtendedStyle(DWORD dwExtendedStyle) 
		{
			DWORD oldExtendedStyle = m_dlgItemTemplate.dwExtendedStyle;
			m_dlgItemTemplate.style = dwExtendedStyle;
			return oldExtendedStyle;
		}

		CPoint SetPosition(WORD x, WORD y)
		{
			CPoint oldPosition = CPoint(m_dlgItemTemplate.x, m_dlgItemTemplate.y);
			m_dlgItemTemplate.x = x;
			m_dlgItemTemplate.y = y;
			return oldPosition;
		}

		CSize SetSize(WORD cx, WORD cy)
		{
			CSize oldSize = CSize(m_dlgItemTemplate.cx, m_dlgItemTemplate.cy);
			m_dlgItemTemplate.cx = cx;
			m_dlgItemTemplate.cy = cy;
			return oldSize;
		}

		WORD SetID(WORD id) 
		{ 
			WORD oldID = m_dlgItemTemplate.id; 
			m_dlgItemTemplate.id = id; 
			return oldID;
		}

		WORD GetDlgItemTemplate(DLGITEMTEMPLATE** ppDlgItemHeader)
		{
			*ppDlgItemHeader = &m_dlgItemTemplate;
			return (WORD) sizeof(m_dlgItemTemplate);
		}


		const WCHAR* GetControlClass() { return (WCHAR*)m_pControlClass; }
		const WCHAR* GetCaption() { return (WCHAR*)m_pCaption; }

		BOOL SetControlClassAtom(WORD controlClass);
		BOOL SetControlClassA(const char* controlClass = NULL);
		BOOL SetControlClassW(const WCHAR* controlClass = NULL);

		BOOL SetCaptionID(WORD wResourceID);
		BOOL SetCaptionA(const char* szCaption);
		BOOL SetCaptionW(const WCHAR* szCaption);

		WORD GetInitData(const WORD** ppInitData)
		{
			*ppInitData = (WORD*) m_pInitData;
			return (WORD)m_nInitDataLength;
		}

		BOOL SetInitData(WORD nInitDataLength, const WORD* pInitData);

		BOOL CreateControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    		WORD controlClass, const TCHAR* szCaption = NULL, WORD nInitDataLength = 0,
    		WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);		

		BOOL CreateControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    		const TCHAR* controlClass, const TCHAR* szCaption = NULL, WORD nInitDataLength = 0,
    		WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);

		BOOL CreateControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    		const TCHAR* controlClass, WORD wResourceID, WORD nInitDataLength = 0,
    		WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);

		BOOL CreateControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    		WORD controlClass, WORD wResourceID, WORD nInitDataLength = 0,
    		WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);		

		void CreateControlInMemory(BYTE* pMemory);

		BOOL CreateControlFromMemory(BYTE* p);

	// Attributes
	public:

		UINT GetLength() { 
			return AlignDWord(sizeof(m_dlgItemTemplate) 
				+ m_nControlClassLength
				+ m_nCaptionLength
				+ m_nInitDataLength
				+ sizeof(WORD)
			);
		}

	protected:
		DLGITEMTEMPLATE m_dlgItemTemplate;
		UINT  m_nControlClassLength;
		BYTE* m_pControlClass;
		UINT  m_nCaptionLength;
		BYTE* m_pCaption;
		UINT  m_nInitDataLength;
		BYTE* m_pInitData;

	// Operations
	public:

	// Implementation
	public:
		virtual ~CDlgItemTempl();
		virtual void Serialize( CArchive& ar );

	protected:
		inline BOOL ValidControl(WORD wValue) 
		{ 
			return (wValue >= CONTROL_CLASS_FIRST && wValue <= CONTROL_CLASS_LAST);
		}

		ULONG AlignDWord(ULONG uLong) {	return ((uLong + 3) & ~3); }


	// Following sets up the structures required for adding runtime 
	// information to the embedded class
	// Normally done for by DECLARE_SERIAL
	protected: 
		static CRuntimeClass* __stdcall _GetBaseClass(); 
		
	public: 
			static  CRuntimeClass classCDlgItemTempl; 
			virtual CRuntimeClass* GetRuntimeClass() const; 
			static CObject* __stdcall CreateObject(); 
			friend CArchive& __stdcall operator>>(CArchive& ar, CDlgItemTempl* &pOb);

//	DECLARE_SERIAL(CDlgTempl::CDlgItemTempl)


	#ifdef _DEBUG
	public:
		void Dump( CDumpContext& dc ) const;
	#endif

	};
// Construction
public:
	CDlgTempl();

// Attributes
public:
protected:
    // handle to store the memory allocated by LockTemplate
	HLOCAL m_hDlgTemplate;
    // pointer used by LockTemplate to keep track of the memory
    // returned by LocalLock in the LockTemplate function
	void FAR* m_lpDlgTemplate;

    // List of controls on the dialog
    CObList controlList;

    // Constant part of the dialog template
    DLGTEMPLATE m_dlgTemplate;

    // Lenght of the various variable length strings in bytes
   	UINT m_nMenuNameLength;
	UINT m_nClassNameLength;
	UINT m_nCaptionLength;
	UINT m_nFaceNameLength;
    
    // Point size of the font
    WORD m_nPointSize;

    // Variable length data for the template
    // They point to NULL terminated UNICODE strings
    void* m_pMenuName;
    void* m_pClassName;
    void* m_pCaption;
    void* m_pFaceName;


// Operations
public:
    // Functions to add controls to the template
		
    BOOL AddControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  wID, DWORD style,
    	WORD controlClass, const TCHAR* szCaption = NULL, WORD nInitDataLength = 0,
    	WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);		

	BOOL AddControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    	const TCHAR* controlClass, const TCHAR* szCaption = NULL, WORD nInitDataLength = 0,
    	WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);

	BOOL AddControl( WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  id, DWORD style,
    	const TCHAR* controlClass, WORD wOrd, WORD nInitDataLength = 0,
    	WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);
	
    BOOL AddControl(WORD  x, WORD  y, WORD  cx, WORD  cy, WORD  wID, DWORD style,
    	WORD controlClass, WORD wOrd, WORD nInitDataLength = 0,
    	WORD* pInitData = NULL, DWORD dwExtendedStyle = 0);		

    // Create the template in memory so that it can be used with CreateDialogIndirect
    void* LockTemplate();

    // Remove the template from memory that was previously created by LockTemplate
    void UnlockTemplate();

    // Helper function to create the template in memory.
    // This function can be directly used if a valid pointer is passed in.
    BOOL CreateTemplateInMemory(void* pMemory = NULL);

    // Helper function to parse a dialog template in memory
    // This function can be directly used if a pointer a valid template in memory is passed in.
    BOOL CreateTemplateFromMemory(BYTE* p);

    // Returns the size of the template when it is created in memory
    UINT GetLength();
    
    // Get the lenght of just the dialog header without the controls
    UINT GetHeaderLength() 
    {
        return AlignDWord( sizeof(DLGTEMPLATE) +
        m_nMenuNameLength
        + m_nClassNameLength
        + m_nCaptionLength
        + ((m_nFaceNameLength) ? (m_nFaceNameLength + sizeof(WORD)) : 0));
    }

    // retrurns a pointer to a UNICODE string which specifies the menu.
    // if the UNICODE character is 0xFFFF then the next WORD specified the
    // the ID of the menu resource
    const WORD* GetMenuName() { return (WORD*) m_pMenuName; }

    // retrurns a pointer to a UNICODE string which specifies the windows calss name.
    // if the UNICODE character is 0xFFFF then the next WORD specified the
    // the atom of a registered class name
    const WCHAR* GetClassName() { return (WCHAR*) m_pClassName; }

    // retrurns a pointer to a UNICODE string which specifies the windows caption.
    // if the UNICODE character is 0xFFFF then the next WORD specified the
    // the id of the resource to be used with static controls
    const WCHAR* GetCaption() { return (WCHAR*) m_pCaption; }
    
    // Returns the font information used by the template
    // The fontname is returned in ppFontInfo and is a UNICODE string
    // the return value is the point size of the font
    WORD GetFontInfo(const WCHAR** ppFontInfo) 
    {
        ASSERT(*ppFontInfo);

        *ppFontInfo = (WCHAR*) m_pFaceName;
        return m_nPointSize;
    }

    BOOL SetDlgTemplate(DWORD style, 
        DWORD dwExtendedStyle, 
        short x, short y, 
        short cx, short cy);

    BOOL SetMenuID(WORD wResourceID);
    BOOL SetMenuNameA(const char* szMenuName);
    BOOL SetMenuNameW(const WCHAR* szMenuName);

    BOOL SetClassAtom(WORD wClassAtom);
    BOOL SetClassNameA(const char* szClassName);
    BOOL SetClassNameW(const WCHAR* szClassName);
    
    BOOL SetCaptionA(const char* szCaption);
    BOOL SetCaptionW(const WCHAR* szCaption);

    BOOL SetFontInfoA(WORD wPointSize, const char* szFaceName);
    BOOL SetFontInfoW(WORD wPointSize, const WCHAR* szFaceName);

    void ResetAll(BOOL bWarn = TRUE);

    enum {byItemID, byItemPosition};

    CDlgItemTempl* FindControl(UINT id, UINT nFlag = byItemID);
    UINT FindControlByID(UINT id);
    CDlgItemTempl* FindControlByPosition(UINT position);

    BOOL RemoveControl(UINT id, UINT nFlags, BOOL bDelete = TRUE, CDlgItemTempl** ppControl = NULL);

    BOOL MoveControl(UINT id, UINT nFlags, UINT nPosition);

    BOOL FromResource(UINT nIDTemplate)
    {
        return FromResource(MAKEINTRESOURCE(nIDTemplate));
    }

    BOOL FromResource(LPCTSTR lpszTemlateName);

// Implementation
public:
	virtual ~CDlgTempl();

public:
	virtual void Serialize( CArchive& ar );
#ifdef _DEBUG
    void Dump( CDumpContext& dc ) const;
#endif


protected:
	ULONG AlignDWord(ULONG uLong) {	return ((uLong + 3) & ~3); }

    DECLARE_SERIAL(CDlgTempl)
};

/////////////////////////////////////////////////////////////////////////////

// Map functions to UNICODE or ANSI depending on the build.
// the functions with out the W or A suffix can be used and
// type of string passed in depends on the build
// The W or A function can also be called as long as the
// correct type of string is passed in.

#ifdef UNICODE

#define SetCaption      SetCaptionW
#define SetClassName    SetClassNameW
#define SetFontInfo     SetFontInfoW
#define SetMenuName     SetMenuNameW

#else

#define SetCaption      SetCaptionA
#define SetClassName    SetClassNameA
#define SetFontInfo     SetFontInfoA
#define SetMenuName     SetMenuNameA

#endif

