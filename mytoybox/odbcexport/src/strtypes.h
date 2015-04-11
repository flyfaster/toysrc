#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <sqltypes.h>

#ifdef _UNICODE
typedef wchar_t my_tchar;
#else
typedef char my_tchar;

#endif

typedef std::basic_string<my_tchar> tstring;
typedef std::basic_stringstream<my_tchar> tstringstream;
typedef std::basic_ostream<my_tchar> tostream;
extern tostream& tout;

std::wstring str2wstr(const std::string &s, const std::locale &loc = std::locale());
std::string wstr2str(const std::wstring &s, const std::locale &loc = std::locale());

tstring convert(const std::string& st);

tstring convert(const std::wstring& st);

std::string tstr2str(const tstring& aa);

class wxString;
tstring convert(const wxString& ws);

template<class string_type>
string_type sqlstr2tstr(const SQLTCHAR* sql_str)
{
    int len=0;
    const SQLTCHAR* pos = sql_str;
    while (*pos)
        pos++;
    len = pos-sql_str;
    string_type ts(len+1, 0);
    for (int i=0; i<len; i++)
        ts[i] = sql_str[i];
    return ts;
}
