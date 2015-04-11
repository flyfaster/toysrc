///////////////////////////////////////////////////////////////////////////////
// XTIB.H  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// Original TIB.H file from Microsoft Systems Journal "Under the Hood",
// May 1996, by Matt Pietrek
//
// Portions copied from Microsoft Systems Journal "Bugslayer Column",
// Feb 2000, by John Robbins
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XTIB_H
#define XTIB_H

// Call SetThreadName(), and then paste this expression into the watch window:
//    (char*)(DW(@TIB+0x14))

#pragma pack(1)

typedef struct _EXCEPTION_REGISTRATION_RECORD
{
	struct _EXCEPTION_REGISTRATION_RECORD * pNext;
	FARPROC 								pfnHandler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

typedef struct tagXTIB
{
	PEXCEPTION_REGISTRATION_RECORD pvExcept; // 00h Head of exception record list
	PVOID	pvStackUserTop; 			// 04h Top of user stack
	PVOID	pvStackUserBase;			// 08h Base of user stack

	union								// 0Ch (NT/Win95 differences)
	{
		struct							// Win95 fields
		{
			WORD	pvTDB;				// 0Ch TDB
			WORD	pvThunkSS;			// 0Eh SS selector used for thunking to 16 bits
			DWORD	unknown1;			// 10h
		} WIN95;

		struct	// WinNT fields
		{
			PVOID SubSystemTib; 		// 0Ch
			ULONG FiberData;			// 10h
		} WINNT;
	} TIB_UNION1;

	PVOID	pvArbitrary;				// 14h Available for application use
	struct _tib *ptibSelf;				// 18h Linear address of TIB structure

	union								// 1Ch (NT/Win95 differences)
	{
		struct	// Win95 fields
		{
			WORD	TIBFlags;			// 1Ch
			WORD	Win16MutexCount;	// 1Eh
			DWORD	DebugContext;		// 20h
			DWORD	pCurrentPriority;	// 24h
			DWORD	pvQueue;			// 28h Message Queue selector
		} WIN95;

		struct	// WinNT fields
		{
			DWORD unknown1; 			// 1Ch
			DWORD processID;			// 20h
			DWORD threadID; 			// 24h
			DWORD unknown2; 			// 28h
		} WINNT;
	} TIB_UNION2;

	PVOID*	pvTLSArray; 				// 2Ch Thread Local Storage array

	union								// 30h (NT/Win95 differences)
	{
		struct	// Win95 fields
		{
			PVOID*	pProcess;			// 30h Pointer to owning process database
		} WIN95;
	} TIB_UNION3;


	// Internal function to get the TIB
	/*----------------------------------------------------------------------
	FUNCTION    :   GetTIB

	DESCRIPTION :   Returns pointer to TIB for current thread.

	PARAMETERS  :   None.

	RETURNS     :   NULL  - A really bad thing
                    !NULL - Pointer to TIB for current thread
	----------------------------------------------------------------------*/
	static tagXTIB * GetTIB()
	{
		tagXTIB * pTib;

		__asm
		{
			MOV  EAX , FS:[18h]
			MOV  pTib , EAX
		}
		return pTib;
	}

	/*----------------------------------------------------------------------
	FUNCTION    :   SetThreadName

	DESCRIPTION :   SetThreadName provides a way to "name" your threads so that you can
                    see at a glance which thread is active when you are in the debugger.
                    Calling SetThreadName sets the string pointer parameter into the Thread
                    Information Block (TIB) pvArbitrary field (offset 0x14).  Matt Pietrek
                    discussed the TIB structure in his May 1996 "Under the Hood" column.
                    I included Matt's TIB.h as part of this project if you want to see the
                    rest of the fields in the TIB.
                    pvArbitrary is an unused spot in the TIB that applications can use
                    as they wish.  SetThreadName does the right thing and checks if the
                    pvArbitrary is not 0 and will not write the string pointer to avoid
                    tromping on any other data written there.
                    To view which thread is active in the Watch window, use
                    "(char*)(DW(@TIB+0x14))"  As you swap threads, you can now tell at a
                    glance which thread you are in!

	PARAMETERS  :   szName - A pointer to the string which you would like to name the
                    current thread.  You should make the string pointer a
                    constant name.

	RETURNS     :   TRUE  - The thread name was set.
                    FALSE - Something else overwrote the pvArbitrary field.
	----------------------------------------------------------------------*/
	static BOOL SetThreadName(LPCTSTR szName)
	{
		// Grab the TIB.
		tagXTIB * pTib = GetTIB();

		// If someone has already written to the arbitrary field, I don't
		// want to be overwriting it.
		if (pTib->pvArbitrary != NULL)
			return FALSE;

		// Nothing's there.  Set the name.
		pTib->pvArbitrary = (void*)szName;

		return TRUE;
	}

	/*----------------------------------------------------------------------
	FUNCTION    :   GetThreadName

	DESCRIPTION :   Returns the string pointer to the name assigned to the
                    current thread.

	PARAMETERS  :   None.

	RETURNS     :   NULL  - No name was assigned.
                    !NULL - The value at the TIB pvArbitrary offset.  Please
                    note that the pointer could be invalid of if something
                    other than SetThreadName used the pvArbitrary offset.
	----------------------------------------------------------------------*/
	static LPCTSTR GetThreadName()
	{
		// Grab the TIB.
		tagXTIB * pTib = GetTIB();

		return (LPCTSTR)pTib->pvArbitrary;
	}


} XTIB;

#pragma pack()

#endif //XTIB_H
