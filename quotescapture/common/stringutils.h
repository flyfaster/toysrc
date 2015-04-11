#pragma once 
typedef std::basic_string<TCHAR> tchar_string;
tchar_string to_tchar_string(const std::string& src );

tchar_string to_tchar_string(const std::wstring& src );

std::string to_string(const tchar_string& src );
