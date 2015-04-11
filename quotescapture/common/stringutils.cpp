#include "stdafx.h"
#include <string>
#include <algorithm>
#include "stringutils.h"

tchar_string to_tchar_string(const std::string& src )
{
#ifdef _UNICODE
    std::wstring s;
    s.assign(src.begin(), src.end());
    return s;
#else
    return src;
#endif
}

tchar_string to_tchar_string(const std::wstring& src )
{
#ifdef _UNICODE
    return src;
#else
    std::string s;
    s.assign(src.begin(), src.end());
    return s;
#endif
}

std::string to_string(const tchar_string& src )
{
#ifdef _UNICODE
    std::string s;
    s.assign(src.begin(), src.end());
    return s;
#else
    return src;
#endif

}