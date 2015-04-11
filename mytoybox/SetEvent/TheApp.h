#pragma once
#include "afxwin.h"

class CTheApp :
	public CWinApp
{
public:
	BOOL InitInstance();
	int ExitInstance();

	// Override this to provide wxWidgets message loop
	// compatibility
	BOOL PreTranslateMessage(MSG *msg);
	BOOL OnIdle(LONG lCount);
};
