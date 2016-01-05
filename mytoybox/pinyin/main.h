#ifndef __MAIN_HPP__
#define __MAIN_HPP__
#include <deque>
#include <string>
std::basic_string<wchar_t> convert_utf8_to_utf16(const std::string& utf8);

class MainApp {
public:
	static MainApp* Instance();
	 int LoadDict();
	 std::deque<std::wstring> dict;
	 std::wstring chinese_to_pinyin(const std::wstring& chinese);
};
#endif
