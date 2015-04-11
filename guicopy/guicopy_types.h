#pragma once 

typedef std::basic_string<TCHAR> tchar_string;
typedef std::basic_stringstream<TCHAR> tstringstream;
#ifdef _UNICODE
typedef boost::filesystem::wpath tpath;
#define tout std::wcout
#else
typedef boost::filesystem::path tpath;
#define tout std::cout
#endif

//typedef boost::filesystem::basic_path<tchar_string, boost::filesystem::wpath_traits> tpath;
typedef boost::filesystem::basic_directory_iterator<tpath> tdirectory_iterator;

typedef boost::basic_regex<TCHAR> tregex;
typedef boost::match_results<tchar_string::const_iterator > tmatch_results;

