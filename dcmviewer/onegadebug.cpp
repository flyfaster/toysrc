 #include "stdafx.h"
#include ".\onegadebug.h"
#include <atlbase.h> 
#include <fstream>
#include <psapi.h>
#pragma comment(lib,"psapi")
#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <Tlhelp32.h>
#include "dllversion.h"
#include <strsafe.h>

OnegaDebug::OnegaDebug(void)
{

}

OnegaDebug::~OnegaDebug(void)
{

}


bool OnegaDebug::g_bNeedReturn;
 

void OnegaDebug::DumpString(LPCSTR msg)
{
	if(msg)
	{
		if( DUMP_TO_DEBUGGER ==mg_DumpMode)
		{
			OutputDebugStringA(msg);
			if(g_bNeedReturn)
				OutputDebugStringA("\n");
		}
		else
			if(DUMP_TO_CONSOLE == mg_DumpMode)
			{
				std::cout<< msg <<std::endl;
			}
	}
}
void OnegaDebug::DumpString(LPCWSTR msg)
{
	if(msg)
	{
		if( DUMP_TO_DEBUGGER ==mg_DumpMode)
		{
			OutputDebugStringW(msg);
			if(g_bNeedReturn)
				OutputDebugStringW(L"\n");
		}
		else
			if(DUMP_TO_CONSOLE == mg_DumpMode)
			{
				std::wcout<< msg <<std::endl;
			}
	}
}
void OnegaDebug::DumpDlls(LPCTSTR filename)
{
	std::basic_string<TCHAR> file_name = GetApplicationPath()+_T("dlls.html");
	if(filename)
	{
		file_name = filename;
	}
	HANDLE hProcess = NULL;
	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, GetCurrentProcessId() );
	if (NULL == hProcess)
	{
		return;
	}
	try
	{
		TCHAR szModName[PATH_LENGTH];
		std::basic_ofstream<TCHAR> of_dlls;
		of_dlls.open( (LPCSTR)_bstr_t(file_name.c_str()),std::ios_base::out);
		if(of_dlls.good())
		{
			HMODULE hMods[4096];
			DWORD cbNeeded = 0;
			of_dlls<<_T("<html><body><table><tr><td>file name</td><td>load address</td><td>size</td><td>entry point</td><td>version</td></tr>")<<std::endl;
			if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				unsigned int i = 0;
				for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
				{
					if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
						sizeof(szModName)/sizeof(szModName[0])))
					{
						of_dlls<<_T("<tr>");
						of_dlls<< _T("<td>")<<szModName<<_T("</td>") ;
						HMODULE hMod =  hMods[i];
						BOOL bmodule = FALSE;
						if(hMod)
						{
							MODULEINFO module_info;
							bmodule = GetModuleInformation(hProcess,hMod,&module_info,sizeof(module_info));
							if(bmodule)
							{
								of_dlls<<_T("<td>0x")<<std::hex<<std::showbase<<module_info.lpBaseOfDll<<"</td>";
								of_dlls<<_T("<td>")<<std::dec<<module_info.SizeOfImage<<"</td>";
								of_dlls<<_T("<td>0x")<<std::hex<<std::showbase<<module_info.EntryPoint<<"</td>";
							}
						}
						if(!bmodule)
						{
							of_dlls<<_T("<td></td><td></td><td></td>");
						}
						CDLLVersion module_ver((LPTSTR)szModName);
						of_dlls<<_T("<td>")<<(LPCTSTR)module_ver.GetFullVersion()<<_T("</td>");
						of_dlls<<_T("</tr>");
						of_dlls<<std::endl;
					}
				}
			}
			of_dlls<<_T("</table></body></html>");
		}
		of_dlls.close();
		CloseHandle( hProcess );
	}
	catch(...)
	{
		Dump(_T("failed to dump dlls"));
	}
}

LPCTSTR OnegaDebug::VariantTypeToString(VARTYPE vt)
{
	__declspec( thread ) static TCHAR type_name_buf[64];
	std::map<VARTYPE,LPCTSTR> vartype_names;
	memset(type_name_buf,0,sizeof(type_name_buf));
	if(vartype_names.size()<1)
	{
		vartype_names[VT_EMPTY] = _T("VT_EMPTY");
		vartype_names[VT_NULL] = _T("VT_NULL");
		vartype_names[VT_I2] = _T("VT_I2");
		vartype_names[VT_I4] = _T("VT_I4");
		vartype_names[VT_R4] = _T("VT_R4");
		vartype_names[VT_R8] = _T("VT_R8");
		vartype_names[VT_CY] = _T("VT_CY");
		vartype_names[VT_DATE] = _T("VT_DATE");
		vartype_names[VT_BSTR] = _T("VT_BSTR");
		vartype_names[VT_DISPATCH] = _T("VT_DISPATCH");
		vartype_names[VT_ERROR] = _T("VT_ERROR");
		vartype_names[VT_BOOL] = _T("VT_BOOL");
		vartype_names[VT_VARIANT] = _T("VT_VARIANT");
		vartype_names[VT_UNKNOWN] = _T("VT_UNKNOWN");
		vartype_names[VT_DECIMAL] = _T("VT_DECIMAL");
		vartype_names[VT_RECORD] = _T("VT_RECORD");
		vartype_names[VT_I1] = _T("VT_I1");
		vartype_names[VT_UI1] = _T("VT_UI1");
		vartype_names[VT_UI2] = _T("VT_UI2");
		vartype_names[VT_UI4] = _T("VT_UI4");
		vartype_names[VT_I8] = _T("VT_I8");
		vartype_names[VT_UI8] = _T("VT_UI8");
		vartype_names[VT_INT] = _T("VT_INT");
		vartype_names[VT_UINT] = _T("VT_UINT");
		vartype_names[VT_INT_PTR] = _T("VT_INT_PTR");
		vartype_names[VT_UINT_PTR] = _T("VT_UINT_PTR");
		vartype_names[VT_VOID] = _T("VT_VOID");
		vartype_names[VT_HRESULT] = _T("VT_HRESULT");
		vartype_names[VT_PTR] = _T("VT_PTR");
		vartype_names[VT_SAFEARRAY] = _T("VT_SAFEARRAY");
		vartype_names[VT_CARRAY] = _T("VT_CARRAY");
		vartype_names[VT_USERDEFINED] = _T("VT_USERDEFINED");
		vartype_names[VT_LPSTR] = _T("VT_LPSTR");
		vartype_names[VT_LPWSTR] = _T("VT_LPWSTR");
		vartype_names[VT_FILETIME] = _T("VT_FILETIME");
		vartype_names[VT_BLOB] = _T("VT_BLOB");
		vartype_names[VT_STREAM] = _T("VT_STREAM");
		vartype_names[VT_STORAGE] = _T("VT_STORAGE");
		vartype_names[VT_STREAMED_OBJECT] = _T("VT_STREAMED_OBJECT");
		vartype_names[VT_STORED_OBJECT] = _T("VT_STORED_OBJECT");
		vartype_names[VT_VERSIONED_STREAM] = _T("VT_VERSIONED_STREAM");
		vartype_names[VT_BLOB_OBJECT] = _T("VT_BLOB_OBJECT");
		vartype_names[VT_CF] = _T("VT_CF");
		vartype_names[VT_CLSID] = _T("VT_CLSID");
		vartype_names[VT_VECTOR] = _T("VT_VECTOR");
		vartype_names[VT_ARRAY] = _T("VT_ARRAY");
		vartype_names[VT_BYREF] = _T("VT_BYREF");
		vartype_names[VT_BSTR_BLOB] = _T("VT_BSTR_BLOB");	
	}

	VARTYPE type_hi = vt &0xf000;
	VARTYPE type_low = vt & 0xfff;
	if( type_hi!=0 && vartype_names.find(type_hi)!=vartype_names.end())
	{
		StringCchCopy(type_name_buf,ARRAY_SIZE(type_name_buf), vartype_names[type_hi]);
		StringCchCat(type_name_buf,ARRAY_SIZE(type_name_buf),_T("|"));
	}
	if(type_low!=0 && vartype_names.find(type_low)!=vartype_names.end())
	{
		StringCchCat(type_name_buf,ARRAY_SIZE(type_name_buf),vartype_names[type_hi]);
	}
	return type_name_buf;
}

tchar_string OnegaDebug::GetApplicationPath(void)
{
	TCHAR buf[PATH_LENGTH];
	buf[0] = _T('\0');
	int buf_length = sizeof(buf)/sizeof(buf[0]);
	DWORD dwret = GetModuleFileName(NULL,buf, buf_length);
	if(dwret == buf_length)
	{
		buf[0] = 0;
	}
	return tchar_string(buf);
}

tchar_string OnegaDebug::GetApplicationName(void)
{
	tchar_string file_name;
	tchar_string application_path = GetApplicationPath();
	tchar_string::size_type find_position = application_path.rfind(_T('\\'));
	if(find_position != tchar_string::npos)
	{
		file_name = application_path.substr(find_position+1);
	}
	return file_name;
}

tchar_string OnegaDebug::GetApplicationDirectory(void)
{
	tchar_string directory;
	tchar_string application_path = GetApplicationPath();
	tchar_string::size_type find_position = application_path.rfind(_T('\\'));
	if(find_position != tchar_string::npos)
	{
		directory = application_path.substr(0,find_position+1);
	}
	return directory;
}
OnegaDebug::DUMP_MODE OnegaDebug::mg_DumpMode = OnegaDebug::DUMP_TO_DEBUGGER;
void OnegaDebug::SetDumpMode(DUMP_MODE a)
{
	mg_DumpMode = a;
}
tstringstream OnegaDebug::mg_cout;

namespace OnegaDebugNS
{
	tstringstream &cout = OnegaDebug::mg_cout;
	OnegaEndl endl;
};
namespace std
{
	tostream& operator<<(tostream& a, OnegaEndl el)
	{
		a<<std::endl;
		OnegaDebug::Dump(OnegaDebugNS::cout.str().c_str());
		OnegaDebugNS::cout.str(_T(""));
		return a;
	}
}



//int OnegaDebug::Dump(LPCSTR fmt, ...)
//{
//#ifdef _DEBUG
//	static __declspec(thread) char s[2048];
//	static __declspec(thread) int cnt;
//	va_list argptr;
//	va_start(argptr, fmt);
//	cnt = vsprintf(s, fmt, argptr);
//	va_end(argptr);
//	DumpString(s);
//	return(cnt);
//#else
//	return 0;
//#endif
// }
int advance_number(const char *&p)
{
	int ret = atoi(p);
	const char S_NULL = char('\0');
	for (; *p != S_NULL && isdigit(*p);	p ++)
	{
		;
	}
	return ret;
}
int advance_number(const wchar_t *&p)
{
	int ret = _wtoi(p);
	const wchar_t S_NULL = wchar_t('\0');
	for (; *p != S_NULL && iswdigit(*p);	p ++)
	{
		;
	}
	return ret;
}
int OnegaDebug::Dump(LPCWSTR fmt, ...)
{
//#ifdef _DEBUG
//	static __declspec(thread) wchar_t s[2048];
//	static __declspec(thread) int cnt;
//	va_list argptr;
//	va_start(argptr, fmt);
//	cnt = vswprintf(s, fmt, argptr);
//	va_end(argptr);
//	DumpString(s);
//	return(cnt);
//#else
//	return 0;
//#endif
	std::wstringstream ts;
#define TSTR(X)  L##X
typedef wchar_t	CHAR_TYPE;	
	if(NULL == fmt)
	{
		OnegaDebug::DumpString(TSTR("(null)"));
		return 0;
	}
	const CHAR_TYPE S_NULL = CHAR_TYPE('\0');
	const CHAR_TYPE *p = NULL;
	va_list argp;
	int i =0;
	CHAR_TYPE *s = NULL;
	CHAR_TYPE c= S_NULL;
	double f =0;
	bool b_error = false;
	va_start(argp, fmt);
	for(p = fmt; *p != S_NULL; p++)
	{
		if(*p != CHAR_TYPE('%') )
		{
			c = *p;
			ts<< c;
			continue;
		}
		else
		{
			p++; //skip %
		}
		int width = 0;
		const int INVALID_PRECISION = -1;
		int precision = INVALID_PRECISION;
		const CHAR_TYPE* prefix = NULL;
		//eat flag here: More than one flag can appear in a format specification. 
		int flag_0 = 0;
		int flag_left = 0;
		int flag_sign = 0;
		int flag_space =0;
		int flag_hash = 0;
		int flag_star =0;
		for (; *p != S_NULL; p ++)
		{
			bool flag_end = false;
			CHAR_TYPE next_c = *p;
			switch(next_c) {
			case CHAR_TYPE('0'):
				flag_0++;
				break;
			case CHAR_TYPE('-'):
				flag_left++;
				break;
			case CHAR_TYPE('+'):
				flag_sign++;
				break;
			case CHAR_TYPE(' '):
				flag_space++;
				break;
			case CHAR_TYPE('#'):
				flag_hash++;
				break;
			case CHAR_TYPE('*'): // this is width field
				flag_star++;
				width = va_arg(argp, int);
				flag_end = true;
				break;
			default:
				flag_end =true;
				break;
			}
			if(flag_end)
				break;
		}
		if(flag_left>0)
		{
			ts<<std::left;
		}
		else
		{
			ts<<std::right;
		}
		if(flag_0>0 && flag_left == 0)
		{
			ts<<std::setfill(CHAR_TYPE('0'));
		}
		if(flag_hash>0)
		{
			ts<<std::showbase;
		}
		else
		{
			ts<<std::noshowbase;
		}
		if(flag_sign>0)
		{
			ts<<std::showpos;
		}
		else
		{
			ts << std::noshowpos;
		}
		//eat width here:
		if( 0== width)
		{
			width = advance_number(p);
		}
		if( width>0)
			ts<<std::setw(width);
		else
			ts<<std::setw(0);
		//eat precision here:
		if (*p == CHAR_TYPE('.') )
		{
			p++;
			if (*p == CHAR_TYPE('*'))
			{ //
				precision = va_arg(argp, int);
				p ++; 
			}
			else
			{  
				precision = advance_number(p);
			}			
		}
		if(precision>0)
		{
			ts<<std::fixed<<std::setprecision(precision);
		}
		//eat prefix:
		CHAR_TYPE prefix_buf[3];
		memcpy( (void*)prefix_buf,p, sizeof(prefix_buf));
		int prefix_h =0;
		int prefix_L = 0;
		int prefix_I = 0;
		int prefix_I64 =0;
		int prefix_I32 =0;
		if( CHAR_TYPE('h')==prefix_buf[0] )
		{
			p++;
			prefix_h = 1;
		}
		else
			if (CHAR_TYPE('I')==prefix_buf[0] )
			{
			p++;
			prefix_I = 1;

			}
			else
				if( CHAR_TYPE('L') == prefix_buf[0])
				{
					p++;
					prefix_L = 1;
				}
		else
			if(CHAR_TYPE('I')==prefix_buf[0] && CHAR_TYPE('6')==prefix_buf[1] && CHAR_TYPE('4')==prefix_buf[2])
			{
				p += 3; // skip I64
				prefix_I64 = 1;
			}
			else
				if(CHAR_TYPE('I')==prefix_buf[0] && CHAR_TYPE('3')==prefix_buf[1] && CHAR_TYPE('2')==prefix_buf[2])
				{
					p += 3;
					prefix_I32 = 1;
				}
			CHAR_TYPE type = *p;
			switch(type)
			{
			case CHAR_TYPE('c'):
			case CHAR_TYPE('C'):
				c = va_arg(argp, int);
				ts << c;
				break;

			case CHAR_TYPE('d'):
			case CHAR_TYPE('i'):
				i = va_arg(argp, int);
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				if(flag_space>0 && flag_sign == 0 && i>0)
				{
					ts<<CHAR_TYPE(' ');
				}
				ts<<std::dec << i;
				break;
			case CHAR_TYPE('o'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::oct << (unsigned int)i;
				break;
			case CHAR_TYPE('u'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::dec << (unsigned int)i;
				break;
			case CHAR_TYPE('x'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::hex<<std::nouppercase <<(unsigned int)i;
				break;
			case CHAR_TYPE('X'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<< std::hex<<std::uppercase <<(unsigned int)i;
				break;
			case CHAR_TYPE('e'):
			case CHAR_TYPE('E'):
			case CHAR_TYPE('f'):
			case CHAR_TYPE('g'):
			case CHAR_TYPE('G'):
				if( INVALID_PRECISION== precision) // e E f Default precision is 1
				{
					ts<<std::setprecision(6);
				}
				f = va_arg(argp, double);
				ts << f;
				break;
			case CHAR_TYPE('n'):
				//Number of characters successfully written so far to the stream or buffer; 
				//this value is stored in the integer whose address is given as the argument.
				{
					int *tmpn = NULL;
					tmpn = va_arg(argp, int*);
					if(tmpn)
					{
						*tmpn = (int)ts.str().length();
					}
					else
					{
						//b_error = true;
					}
				}
				break;
			case CHAR_TYPE('p'):
				{
					void* tmpp = NULL;
					tmpp = va_arg(argp, void *);
					ts<<std::hex<< tmpp;
				}
				break;
			case CHAR_TYPE('s'):
			case CHAR_TYPE('S'):
				s = va_arg(argp, CHAR_TYPE *);
				if(NULL==s)
				{
					ts << TSTR("(null)");
				}
				else
				{
					ts << s;
				}
				break;

			case CHAR_TYPE('%'):
				ts << CHAR_TYPE("%");
				break;
			default:
				b_error = true;
				ts << TSTR(" invalid_flag found:")<<type;
				break;
			}
			if(b_error)
			{
				break;
			}
	}
	va_end(argp);
	OnegaDebug::DumpString(ts.str().c_str());
	return (int)ts.str().length();
#undef TSTR
}
int OnegaDebug::Dump(const char *fmt, ...)
{//%[flags] [width] [.precision] [{h | l | I64 | L}]type
	std::stringstream ts;
#define TSTR(X)  X
typedef char	CHAR_TYPE;	
	if(NULL == fmt)
	{
		OnegaDebug::DumpString(TSTR("(null)"));
		return 0;
	}
	const CHAR_TYPE S_NULL = CHAR_TYPE('\0');
	const CHAR_TYPE *p = NULL;
	va_list argp;
	int i =0;
	CHAR_TYPE *s = NULL;
	CHAR_TYPE c= S_NULL;
	double f =0;
	bool b_error = false;
	va_start(argp, fmt);
	for(p = fmt; *p != S_NULL; p++)
	{
		if(*p != CHAR_TYPE('%') )
		{
			c = *p;
			ts<< c;
			continue;
		}
		else
		{
			p++; //skip %
		}
		int width = 0;
		const int INVALID_PRECISION = -1;
		int precision = INVALID_PRECISION;
		const CHAR_TYPE* prefix = NULL;
		//eat flag here: More than one flag can appear in a format specification. 
		int flag_0 = 0;
		int flag_left = 0;
		int flag_sign = 0;
		int flag_space =0;
		int flag_hash = 0;
		int flag_star =0;
		for (; *p != S_NULL; p ++)
		{
			bool flag_end = false;
			CHAR_TYPE next_c = *p;
			switch(next_c) {
			case CHAR_TYPE('0'):
				flag_0++;
				break;
			case CHAR_TYPE('-'):
				flag_left++;
				break;
			case CHAR_TYPE('+'):
				flag_sign++;
				break;
			case CHAR_TYPE(' '):
				flag_space++;
				break;
			case CHAR_TYPE('#'):
				flag_hash++;
				break;
			case CHAR_TYPE('*'): // this is width field
				flag_star++;
				width = va_arg(argp, int);
				flag_end = true;
				break;
			default:
				flag_end =true;
				break;
			}
			if(flag_end)
				break;
		}
		if(flag_left>0)
		{
			ts<<std::left;
		}
		else
		{
			ts<<std::right;
		}
		if(flag_0>0 && flag_left == 0)
		{
			ts<<std::setfill(CHAR_TYPE('0'));
		}
		if(flag_hash>0)
		{
			ts<<std::showbase;
		}
		else
		{
			ts<<std::noshowbase;
		}
		if(flag_sign>0)
		{
			ts<<std::showpos;
		}
		else
		{
			ts << std::noshowpos;
		}
		//eat width here:
		if( 0== width)
		{
			width = advance_number(p);
		}
		if( width>0)
			ts<<std::setw(width);
		else
			ts<<std::setw(0);
		//eat precision here:
		if (*p == CHAR_TYPE('.') )
		{
			p++;
			if (*p == CHAR_TYPE('*'))
			{ //
				precision = va_arg(argp, int);
				p ++; 
			}
			else
			{  
				precision = advance_number(p);
			}			
		}
		if(precision>0)
		{
			ts<<std::fixed<<std::setprecision(precision);
		}
		//eat prefix:
		CHAR_TYPE prefix_buf[3];
		memcpy( (void*)prefix_buf,p, sizeof(prefix_buf));
		int prefix_h =0;
		int prefix_L = 0;
		int prefix_I = 0;
		int prefix_I64 =0;
		int prefix_I32 =0;
		if( CHAR_TYPE('h')==prefix_buf[0] )
		{
			p++;
			prefix_h = 1;
		}
		else
			if (CHAR_TYPE('I')==prefix_buf[0] )
			{
			p++;
			prefix_I = 1;

			}
			else
				if( CHAR_TYPE('L') == prefix_buf[0])
				{
					p++;
					prefix_L = 1;
				}
		else
			if(CHAR_TYPE('I')==prefix_buf[0] && CHAR_TYPE('6')==prefix_buf[1] && CHAR_TYPE('4')==prefix_buf[2])
			{
				p += 3; // skip I64
				prefix_I64 = 1;
			}
			else
				if(CHAR_TYPE('I')==prefix_buf[0] && CHAR_TYPE('3')==prefix_buf[1] && CHAR_TYPE('2')==prefix_buf[2])
				{
					p += 3;
					prefix_I32 = 1;
				}
			CHAR_TYPE type = *p;
			switch(type)
			{
			case CHAR_TYPE('c'):
			case CHAR_TYPE('C'):
				c = va_arg(argp, int);
				ts << c;
				break;

			case CHAR_TYPE('d'):
			case CHAR_TYPE('i'):
				i = va_arg(argp, int);
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				if(flag_space>0 && flag_sign == 0 && i>0)
				{
					ts<<CHAR_TYPE(' ');
				}
				ts<<std::dec << i;
				break;
			case CHAR_TYPE('o'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::oct << (unsigned int)i;
				break;
			case CHAR_TYPE('u'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::dec << (unsigned int)i;
				break;
			case CHAR_TYPE('x'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<<std::hex<<std::nouppercase <<(unsigned int)i;
				break;
			case CHAR_TYPE('X'):
				if( INVALID_PRECISION== precision) // d, i, u, o, x, X Default precision is 1
				{
					ts<<std::setprecision(1);
				}
				i = va_arg(argp, int);
				ts<< std::hex<<std::uppercase <<(unsigned int)i;
				break;
			case CHAR_TYPE('e'):
			case CHAR_TYPE('E'):
			case CHAR_TYPE('f'):
			case CHAR_TYPE('g'):
			case CHAR_TYPE('G'):
				if( INVALID_PRECISION== precision) // e E f Default precision is 1
				{
					ts<<std::setprecision(6);
				}
				f = va_arg(argp, double);
				ts << f;
				break;
			case CHAR_TYPE('n'):
				//Number of characters successfully written so far to the stream or buffer; 
				//this value is stored in the integer whose address is given as the argument.
				{
					int *tmpn = NULL;
					tmpn = va_arg(argp, int*);
					if(tmpn)
					{
						*tmpn = (int)ts.str().length();
					}
					else
					{
						//b_error = true;
					}
				}
				break;
			case CHAR_TYPE('p'):
				{
					void* tmpp = NULL;
					tmpp = va_arg(argp, void *);
					ts<<std::hex<< tmpp;
				}
				break;
			case CHAR_TYPE('s'):
			case CHAR_TYPE('S'):
				s = va_arg(argp, CHAR_TYPE *);
				if(NULL==s)
				{
					ts << TSTR("(null)");
				}
				else
				{
					ts << s;
				}
				break;

			case CHAR_TYPE('%'):
				ts << CHAR_TYPE("%");
				break;
			default:
				b_error = true;
				ts << TSTR(" invalid_flag found:")<<type;
				break;
			}
			if(b_error)
			{
				break;
			}
	}
	va_end(argp);
	OnegaDebug::DumpString(ts.str().c_str());
	return (int)ts.str().length();
#undef TSTR
}

/////////////////////////////////////////////////////////////////////
// DoesFileExist : Determines whether a specified file exists
// Parameters:
//     pszFile - File name to check.
// Returns TRUE if the specified file is found, or FALSE otherwise.
/////////////////////////////////////////////////////////////////////
BOOL OnegaDebug::DoesFileExist(LPCTSTR pszFile)
{
    if (pszFile == NULL)
    {
        return FALSE;
    }

    if (lstrlen(pszFile) > MAX_PATH)
    {
        return FALSE;
    }

    // We don't want any error message box to pop up when we try to test
    // if the required file is available to open for read.
    
    UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    
    HANDLE hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    SetErrorMode(uErrorMode);  // restore error mode
    
    if (INVALID_HANDLE_VALUE  == hFile) 
    {
        return FALSE;    
    }
    
    CloseHandle(hFile);
    return TRUE;
} 
bool can_be_string(_variant_t &data)
{
	if(data.vt != VT_EMPTY && data.vt != VT_NULL && VT_DISPATCH != data.vt)
		return true;
	return false;
}

bool string_to_int(LPCTSTR str, int& data)
{
	//return true if trimed(str) is int, else return false
	if(NULL==str)
		return false;
	std::basic_string<TCHAR> stmp = str;
	int length = (int)stmp.length();
	for(int i=0;i <length; i++)
	{
		TCHAR c = stmp.at(i);
		if(!_istdigit(c))
			return false;
	}
	data = _ttoi(str);
	return true;
}

 bool OnegaDebug::DicomDTToSystemTime(LPCTSTR pdatetimestring, SYSTEMTIME &st)
{
	const int MAX_DT_LENGTH = 14;
	if(!pdatetimestring || _tcslen(pdatetimestring)<MAX_DT_LENGTH)
		return false;
	tchar_string datetimestring = pdatetimestring;
	tchar_string::size_type start_pos = 0;
	tchar_string::size_type count = 4;
	st.wYear = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wMonth = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wDay = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wHour = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wMinute = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wSecond = _ttoi(datetimestring.substr(start_pos,count).c_str());
	return true;

}
 bool OnegaDebug::DicomDAToSystemTime(LPCTSTR pdastring,SYSTEMTIME &st)
{
	if(NULL==pdastring || _tcslen(pdastring)<8)
		return false;
	tchar_string datetimestring = pdastring;
	tchar_string::size_type start_pos = 0;
	tchar_string::size_type count = 4;
	st.wYear = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wMonth = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wDay = _ttoi(datetimestring.substr(start_pos,count).c_str());
	return true;
}
 bool OnegaDebug::DicomTMToSystemTime(LPCTSTR tmstring, SYSTEMTIME &st)
{
	if(tmstring==NULL || _tcslen(tmstring)<6)
		return false;
	tchar_string datetimestring = tmstring;
	tchar_string::size_type start_pos = 0;
	tchar_string::size_type count = 2;
	st.wHour = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wMinute = _ttoi(datetimestring.substr(start_pos,count).c_str());
	start_pos += count;
	count = 2;
	st.wSecond = _ttoi(datetimestring.substr(start_pos,count).c_str());
	return true;
}
void OnegaDebug::SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{

   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName =(szThreadName);
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
   }
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
std::string unicode_to_mbcs(const wchar_t* pszSrc,UINT mbcs_code_page)
{
	std::string s_ret;
	if(NULL == pszSrc|| wcslen(pszSrc)==0)
	{
		AtlTrace(_T("%s<%d>NULL string"),__TFUNCTION__, __LINE__);
		return s_ret;
	}
	int wchar_nums = (int)wcslen(pszSrc)+1;
	int gb_chars = WideCharToMultiByte(mbcs_code_page,0,pszSrc,wchar_nums,NULL,0,NULL,NULL);
	if(gb_chars>0)
	{
		std::auto_ptr<char> gb_buf(new char[gb_chars]);
		int convert_ret = WideCharToMultiByte(mbcs_code_page,0,pszSrc,wchar_nums,gb_buf.get(),gb_chars,NULL,NULL);
		if(convert_ret>0)
			s_ret = gb_buf.get();
		else
		{
			DWORD dw_error = GetLastError();
			AtlTrace(_T("%s<%d>return %d, Error:%d"), __TFUNCTION__, __LINE__, convert_ret,dw_error);
		}
	}
	else
	{
		DWORD dw_error = GetLastError();
		AtlTrace(_T("%s<%d>return %d, Error:%d"), __TFUNCTION__, __LINE__, gb_chars,dw_error);
	}
	return s_ret;
}
std::string OnegaDebug::w2GB18030(wchar_t* pszSrc)
{
	const int GB18030_CP = 54936;
	return unicode_to_mbcs(pszSrc,GB18030_CP);
//	std::string s_ret;
//	if(NULL == pszSrc|| wcslen(pszSrc)==0)
//	{
//		AtlTrace("%s<%d>NULL string",__FUNCTION__, __LINE__);
//		return s_ret;
//	}
//	int wchar_nums = wcslen(pszSrc)+1;
//	int gb_chars = WideCharToMultiByte(GB18030_CP,0,pszSrc,wchar_nums,NULL,0,NULL,NULL);
//	if(gb_chars>0)
//	{
//		std::auto_ptr<char> gb_buf(new char[gb_chars]);
//		int convert_ret = WideCharToMultiByte(GB18030_CP,0,pszSrc,wchar_nums,gb_buf.get(),gb_chars,NULL,NULL);
//		if(convert_ret>0)
//			s_ret = gb_buf.get();
//		else
//		{
//			DWORD dw_error = GetLastError();
//			AtlTrace("%s<%d>return %d, Error:%d", __FUNCTION__, __LINE__, convert_ret,dw_error);
//		}
//	}
//	else
//	{
//		DWORD dw_error = GetLastError();
//		AtlTrace("%s<%d>return %d, Error:%d", __FUNCTION__, __LINE__, gb_chars,dw_error);
//	}
//	return s_ret;
}
std::wstring mbcs_to_unicode(LPCSTR pszSrc, UINT code_page)
{
	std::wstring s_ret;
	if(NULL == pszSrc|| strlen(pszSrc)==0)
	{
		AtlTrace(_T("%s<%d>NULL string"),__TFUNCTION__, __LINE__);
		return s_ret;
	}
	int char_nums = (int)strlen(pszSrc)+1;
	//
	int w_chars = MultiByteToWideChar(code_page,0,pszSrc,char_nums,NULL,0);
	if(w_chars>0)
	{
		std::auto_ptr<wchar_t> gb_buf(new wchar_t[w_chars]);
		int convert_ret = MultiByteToWideChar(code_page,0,pszSrc,char_nums,gb_buf.get(),w_chars);
		if(convert_ret>0)
			s_ret = gb_buf.get();
		else
		{
			DWORD dw_error = GetLastError();
			AtlTrace(_T("%s<%d>return %d, Error:%d"), __TFUNCTION__, __LINE__, convert_ret,dw_error);
		}
	}
	else
	{
		DWORD dw_error = GetLastError();
		AtlTrace(_T("%s<%d>return %d, Error:%d"), __TFUNCTION__, __LINE__, w_chars,dw_error);
	}
	return s_ret;
}
void print_multibyte(LPCSTR caption, LPCSTR test_string)
{
	if(NULL == test_string)
		return;
	std::ostringstream oss;
	oss<<caption<<std::endl;
	for(int i=0;i< (int)strlen(test_string);i++)
	{
		char c = test_string[i];
		byte b = c;
		oss<<std::setw(2)<<std::hex<<std::setfill('0')<<(int)b<<" ";
	}
	OutputDebugStringA(oss.str().c_str());
	//OutputDebugString(test_string);
}
void print_wchart(LPCSTR caption,LPCWSTR psz)
{
	if(NULL == psz)
		return;
	std::ostringstream oss;
	oss<<caption<<std::endl;
	LPCSTR test_string = (LPCSTR)psz;
	int length = (int)wcslen(psz)*2;
	for(int i=0;i<length;i++)
	{
		char c = test_string[i];
		byte b = c;
		oss<<std::setw(2)<<std::hex<<std::setfill('0')<<(int)b<<_T(" ");
	}
	OutputDebugStringA(oss.str().c_str());
	//OutputDebugStringW(psz);
}

void dump_bytes(LPCTSTR caption, const byte* pbuf, size_t len)
{
	tstringstream oss;
	oss<<caption<<_T(" length:")<< (int)len;
	if(NULL==pbuf)
		oss<<_T("(null)");
	else
	{
		oss<<std::setw(2)<<std::hex<<std::setfill(_T('0'));
		for(size_t i=0;i<len;i++)
		{
			oss<<pbuf[i]<<_T(" ");
		}
	}
	OutputDebugString( oss.str().c_str());
}

