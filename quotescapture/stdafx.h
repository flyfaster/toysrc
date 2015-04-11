// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <iomanip>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include <algorithm>

#ifdef _AFXDLL
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#endif

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/tokenizer.hpp>
//#define USE_LOKI_PIMPL 1
#include <loki/pimpl.h>
#include <loki/Singleton.h>
//#include <tchar.h>
//#include <windows.h>

template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_debugbuf : 
	public std::basic_stringbuf<CharT, TraitsT>
{
public:

	virtual ~basic_debugbuf()
	{
		sync();
	}

protected:

	int sync()
	{
		output_debug_string(str().c_str());
		str(std::basic_string<CharT>());    // Clear the string buffer
		return 0;
	}
	void output_debug_string(const CharT *text) {}
};
template<>
void basic_debugbuf<char>::output_debug_string(const char *text)
{
	::OutputDebugStringA(text);
}

template<>
void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
{
	::OutputDebugStringW(text);
}
template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : 
	public std::basic_ostream<CharT, TraitsT>
{
public:

	basic_dostream() : std::basic_ostream<CharT, TraitsT>
		(new basic_debugbuf<CharT, TraitsT>()) {}
	~basic_dostream() 
	{
		delete rdbuf(); 
	}
};

//#ifndef _CONSOLE
//extern basic_dostream<char>    cerr;
//extern basic_dostream<char>    cout;
//extern basic_dostream<wchar_t> wcerr;
//extern basic_dostream<wchar_t> wcout;
//#else
//using std::cout;
//using std::cerr;
//#endif

#if defined STOCKCLIENTDLL_EXPORTS
#   define STOCKCLIENT_API  __declspec(dllexport)
#   define EXPIMP_TEMPLATE  // refer to http://support.microsoft.com/kb/168958

//void DebugPrint(char * format, ...);

#else
#   define STOCKCLIENT_API  __declspec(dllimport)
#   define EXPIMP_TEMPLATE extern
#endif
