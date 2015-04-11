#include <wx/wx.h>
// #include <sqltypes.h>
#include "strtypes.h"
#pragma warning(disable: 4244)


template<class string_type_src, class string_type_dst>
string_type_dst convert_string(const string_type_src& src)
{
  string_type_dst dst(src.length(), 0);
  std::copy(src.begin(), src.end(),dst.begin());
  return dst;
}

std::wstring str2wstr( const std::string &s, const std::locale &loc /*= std::locale()*/ )
{
//     const std::ctype<wchar_t> &conv(std::use_facet<std::ctype<wchar_t> >(loc));
    //     std::transform(s.begin(), s.end(), std::back_inserter(out),
    //         std::bind1st(std::mem_fun1(std::ctype<wchar_t>::widen), &conv));
//     std::wstring out(s.length(), 0);    
//     std::copy(s.begin(), s.end(),out.begin());
//     return out;
  return convert_string<std::string, std::wstring>(s);
}

std::string wstr2str( const std::wstring &s, const std::locale &loc /*= std::locale()*/ )
{
/*    const std::ctype<wchar_t> &conv(std::use_facet<std::ctype<wchar_t> >(loc));
    std::string out(s.length(), 0);   */ 
    //     std::transform(s.begin(), s.end(), std::back_inserter(out),
    //         std::bind1st(std::mem_fun1(std::ctype<wchar_t>::narrow), &conv));
//     std::copy(s.begin(), s.end(), out.begin());
//     return out;
return convert_string<std::wstring, std::string>(s);
}

tstring str2tstr(const std::string& st )
{
#ifdef _UNICODE
    return str2wstr(st);
#else
    return st;
#endif
}

tstring wstr2tstr(const std::wstring& st )
{
#ifdef _UNICODE
    return st;
#else
    return wstr2str(st);
#endif
}

std::string tstr2str(const tstring& aa )
{
#ifdef _UNICODE
    return wstr2str(aa);
#else
    return aa;
#endif
}
tstring convert(const wxString& ws) {
    return ws.c_str();
// #if wxVERSION_NUMBER < 2811
//     return ws.ToAscii().data();
// #else
//     return ws.ToAscii();
// #endif
}

// tstring sqlstr2tstr(const SQLTCHAR* sql_str) {
//     int len=0;
//     const SQLTCHAR* pos = sql_str;
//     while (*pos)
//         pos++;
//     len = pos-sql_str;
//     tstring ts(len+1, 0);
//     for (int i=0; i<len; i++)
//         ts[i] = sql_str[i];
//     return ts;
// }

#ifdef _UNICODE
tostream& tout=std::wcout;
#else
tostream& tout=std::cout;
#endif