#pragma once
#ifndef _ONEGA_DEBUG_HPP
#define _ONEGA_DEBUG_HPP
#include <windows.h>
#include <string>
#include <sstream>
#include <comdef.h>
#include <boost/tokenizer.hpp>
typedef std::basic_string<TCHAR> tchar_string;


typedef std::basic_ostringstream<TCHAR> tstringstream;
typedef std::basic_ostream<TCHAR> tostream;
class OnegaEndl
{
};

class OnegaDebug
{
public:
	OnegaDebug(void);
	~OnegaDebug(void);
	static int Dump(LPCSTR fmt, ...);
	static int Dump(LPCWSTR fmt, ...);
	static void DumpString(LPCSTR msg);
	static void DumpString(LPCWSTR msg);
	static void DumpDlls(LPCTSTR filename=NULL);
	static LPCTSTR VariantTypeToString(VARTYPE vt);
	static tchar_string GetApplicationPath(void);
	static tchar_string GetApplicationName(void);
	static tchar_string GetApplicationDirectory(void);
	enum DUMP_MODE
	{
		DUMP_TO_DEBUGGER = 4,
		DUMP_TO_FILE = 1,
		DUMP_TO_CONSOLE =2,
		DUMP_STOP = 0,
	};
	static void SetDumpMode(DUMP_MODE a);
	static tstringstream mg_cout;
	static bool g_bNeedReturn;
	static BOOL DoesFileExist(LPCTSTR pszFile);
 	static bool DicomDTToSystemTime(LPCTSTR datetimestring, SYSTEMTIME &st);
	static bool DicomDAToSystemTime(LPCTSTR dastring, SYSTEMTIME &st);
	static bool DicomTMToSystemTime(LPCTSTR tmstring,SYSTEMTIME &st);
	static std::string w2GB18030(wchar_t* pszSrc);
	static void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName);
private:
	static DUMP_MODE mg_DumpMode;
};
const int PATH_LENGTH = MAX_PATH +1024;
namespace std
{
tostream& operator<<(tostream& a, OnegaEndl el);
}
namespace OnegaDebugNS
{
	extern tstringstream &cout;
	extern OnegaEndl endl;
};
void myprintf(const TCHAR *fmt, ...);
bool can_be_string(_variant_t &data);
bool string_to_int(LPCTSTR str, int& data);
 
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

std::wstring mbcs_to_unicode(LPCSTR src, UINT code_page);
std::string unicode_to_mbcs(const wchar_t* pszSrc,UINT mbcs_code_page);

//discompose person name string into DICOM components, including all groups and components, if some group/component is missing, 
//"" is put into the container
//parameters:	person_name	a string representing DICOM person name
//				pn_components		Container of components, can be deque<wstring> or vector<wstring>(15)
//return		bool		always return true
//template<class Wstring_Container_type>
//bool GetPNComponents(LPCWSTR person_name,Wstring_Container_type &pn_components)
//{
//	const size_t MAX_PN_GROUPS = 3;
//	const size_t MAX_PN_COMPONENTS = 5;
//	std::wstring ws_pn =person_name;
//	if(NULL==person_name || ws_pn.length()<1)
//	{
//		while(pn_components.size()<MAX_PN_GROUPS * MAX_PN_COMPONENTS)
//			pn_components.push_back(L"");
//		return true;
//	}
//	std::deque<std::wstring> pn_groups;
//	{
//		typedef boost::tokenizer<boost::char_separator<wchar_t>,std::wstring::const_iterator,std::wstring > tokenizer;
//		boost::char_separator<wchar_t> sep_group( L"=",NULL, boost::keep_empty_tokens);
//		tokenizer group_tokens( ws_pn, sep_group);
//		tokenizer::iterator group_iter = group_tokens.begin();
//		while(group_iter!=group_tokens.end())
//		{
//			std::wstring ws_group = *group_iter;
//			if(ws_group.length()<1)
//				pn_groups.push_back(L"^^^^");
//			else
//				pn_groups.push_back(ws_group);
//			group_iter++;
//		}
//		while(pn_groups.size()<MAX_PN_GROUPS)
//			pn_groups.push_back(L"^^^^");
//		for(int group_index=0;group_index<MAX_PN_GROUPS;group_index++)
//		{
//			std::wstring ws = pn_groups[group_index];
//			boost::char_separator<wchar_t> sep_component( L"^",NULL, boost::keep_empty_tokens);
//			tokenizer component_tokens( ws, sep_component);
//			tokenizer::iterator component_iter = component_tokens.begin();
//			size_t pn_components_num = (group_index+1)*MAX_PN_COMPONENTS;
//			while(component_iter!=component_tokens.end())
//			{
//				std::wstring ws_component = *component_iter;
//				pn_components.push_back(ws_component);
//				component_iter++;
//				if(pn_components.size()>= pn_components_num )
//					break;
//			}
//			while(pn_components.size()<(pn_components_num) )
//				pn_components.push_back(L"");
//		}
//	}
//	return true;
//}
//discompose person name string into DICOM components, including all groups and components, if some group/component is missing, 
//"" is put into the container
//parameters:	person_name	a string representing DICOM person name
//				code_page	should be a valid code page number, such as 54936,1252,etc
//				cont		a container for wstring, can be deque or vector
//return		bool		return true if code page is valid, otherwise return false
//template<class Wstring_Container_type>
//bool GetPNComponents(LPCSTR person_name,UINT code_page, Wstring_Container_type &cont)
//{
//	if(IsValidCodePage(code_page)==FALSE)
//		return false;
//	if(NULL==person_name)
//	{
//		while(cont.size()<15)
//			cont.push_back(L"");
//		return true;
//	}
//	std::wstring ws_pn = mbcs_to_unicode(person_name,code_page);
//	return GetPNComponents(ws_pn.c_str(),cont);
//	
//}
const int GB18030_CP = 54936;

//template<class baseclass>
//class LifeDetector :public baseclass
//{
//public:
//	LifeDetector()
//	{
//		LONG lret = InterlockedIncrement(getCount());
//	}
//	virtual ~LifeDetector()
//	{
//		LONG lret = InterlockedDecrement(getCount());
//	}
//private:
//	long* getCount()
//	{
//		static long volatile instance_count=0;
//		return const_cast<long*>(&instance_count);
//	}
//};
//template<class baseclass, class inputparam>
//class LifeDetector1 :public baseclass
//{
//public:
//	LifeDetector1(inputparam param):baseclass(param)
//	{
//		LONG lret = InterlockedIncrement(getCount());
//	}
//	virtual ~LifeDetector1()
//	{
//		LONG lret = InterlockedDecrement(getCount());
//	}
//private:
//	long* getCount()
//	{
//		static long volatile instance_count=0;
//		return const_cast<long*>(&instance_count);
//	}
//};

void print_multibyte(LPCSTR caption, LPCSTR test_string);
void print_wchart(LPCSTR caption,LPCWSTR psz);
void dump_bytes(LPCSTR caption, const byte* pbuf, size_t len);

#endif
