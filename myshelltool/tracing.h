#pragma once
#ifndef  __tracing_output_debug_string__HPP__
#define __tracing_output_debug_string__HPP__
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>


//  typedef this class when you're looking to implement an auto-tracing macro
//  for a zero-arg format_varargs trace function.  See WXTRACE for more.
class tracing_output_debug_string
{
private:
	const TCHAR* m_file;
	int m_line;

	enum { MY_BUFFER_SIZE = 1024 };
	TCHAR buf1[MY_BUFFER_SIZE];
	TCHAR buf2[MY_BUFFER_SIZE];
	
public:
	tracing_output_debug_string( const TCHAR* file, int line ) :
		m_file( file ),
		m_line( line )
	{
	}
	
	void operator()( const TCHAR* Format, ... )
	{
		buf1[0] = buf2[0] = 0;
		va_list va;
		va_start(va, Format);
		
		//  add the __FILE__ and __LINE__ to the front:
		if(_sntprintf(buf1, MY_BUFFER_SIZE-1, _T("%s:%d %s\n"), m_file, m_line, Format) < 0)
			buf1[MY_BUFFER_SIZE-1] = 0;
		
		//  format the message as requested:
		if(_vsntprintf(buf2, MY_BUFFER_SIZE-1, buf1, va) < 0)
			buf2[MY_BUFFER_SIZE-1] = 0;
		//  write it out!
#ifdef _UNICODE
		OutputDebugStringW( buf2 );
#else
		OutputDebugStringA( buf2 );
#endif
		va_end(va);
	}
};

#ifndef __TFILE__

#define WIDEN2(x) L ## x 
#define WIDEN(x) WIDEN2(x) 
#define __WFILE__ WIDEN(__FILE__) 
#define __WFUNCTION__ WIDEN(__FUNCTION__) 
#ifdef _UNICODE 
#define __TFILE__ __WFILE__ 
#define __TFUNCTION__ __WFUNCTION__ 
#else 
#define __TFILE__ __FILE__ 
#define __TFUNCTION__ __FUNCTION__ 
#endif 

#endif //#ifndef __TFILE__

//#ifdef _DEBUG

#define MYTRACE (tracing_output_debug_string( __TFILE__, __LINE__ ))
#define SC_TRACE (tracing_output_debug_string( __TFILE__, __LINE__ ))
//#else
////#	define MYTRACE __noop
//#define MYTRACE 
//#endif

#endif

