#include <iostream>
#include <fstream>
//#include <codecvt>
#include <boost/locale/encoding_utf.hpp>
#include <string>
#include <boost/locale.hpp>
#include <algorithm>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

#if  !defined( __WXGTK__ ) && !defined(_MSC_VER)
#include "mainwnd.h"
#endif

#ifdef _MSC_VER
#include <Tchar.h>
#include <windows.h>
#include "resource.h"
#endif

#include "main.h"

using boost::locale::conv::utf_to_utf;

std::wstring utf8_to_wstring(const std::string& str)
{
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str)
{
    return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

std::basic_string<wchar_t> convert_utf8_to_utf16(const std::string& utf8) // boost::uint16_t
{
	const wchar_t MALFORMED_CHARACTER=L'.';
	std::basic_string<wchar_t> utf16;
	utf16.resize((utf8.size() + 1) * 2);
	std::basic_string<wchar_t>::iterator utf16_pos = utf16.begin();
	std::string::const_iterator utf8_pos = utf8.begin();
	unsigned int code_point = 0;
	int num_bytes = 0;
	int bytes_remaining = 0;
	while (utf8_pos != utf8.end())
	{
		if (num_bytes == 0)
		{
			if ((0x80 & *utf8_pos) == 0)
			{
				*utf16_pos++ = *utf8_pos++;
			}
			else if((0xE0 & *utf8_pos) == 0xC0)
			{
				code_point = (*utf8_pos++ & 0x1F) << 6;
				num_bytes = 2;
			}
			else if((0xF0 & *utf8_pos) == 0xE0)
			{
				code_point = (*utf8_pos++ & 0x0F) << 12;
				num_bytes = 3;
			}
			else if((0xF8 & *utf8_pos) == 0xF0)
			{
				code_point = (*utf8_pos++ & 0x07) << 18;
				num_bytes = 4;
			}
			else if((0xFC & *utf8_pos) == 0xF8)
			{
				// 5-Byte sequences are illegal, consume it if possible
				code_point = (*utf8_pos++ & 0x03) << 24;
				num_bytes = 5;
			}
			else if((0xFE & *utf8_pos) == 0xFC)
			{
				// 6-Byte sequences are illegal, consume it if possible
				code_point = (*utf8_pos++ & 0x01) << 30;
				num_bytes = 6;
			}
			else
			{
				// Illegal
				*utf16_pos++ = MALFORMED_CHARACTER;
				++utf8_pos;
			}
			if (num_bytes)
			{
				bytes_remaining = num_bytes - 1;
			}
		}
		else
		{
			if (0x80 == (0xC0 & (*utf8_pos)))
			{
				code_point |= (*utf8_pos++ & 0x0000003FL) << ((bytes_remaining - 1) * 6);
				if (--bytes_remaining == 0)
				{
					if ((num_bytes == 2 && code_point < 0x000080)
						|| (num_bytes == 3 && code_point < 0x000800)
						|| (num_bytes == 4 && code_point < 0x010000)
						|| (num_bytes > 4)
						|| ((code_point & 0xFFFFF800) == 0xD800)
						|| (code_point > 0x10FFFF))
					{
						// Illegal character or length
						*utf16_pos++ = MALFORMED_CHARACTER;
					}
					else if(code_point > 0xFFFF)
					{
						// Generate a surrogate pair
						code_point -= 0x00010000;
						*utf16_pos++ = 0xD800 | (0x000003FF & (code_point >>  10));
						*utf16_pos++ = 0xDC00 | (0x000003FF & code_point);
					}
					else if(code_point != 0xFEFF || utf16_pos != utf16.begin())
					{
						// Fits in 1 wide character
						*utf16_pos++ = code_point;
					}
					num_bytes = 0;
				}
			}
			else
			{
				// Illegal sequence
				*utf16_pos++ = MALFORMED_CHARACTER;
				num_bytes = 0;
			}
		}
	}
	utf16.resize(utf16_pos - utf16.begin());
	return utf16;
}

#if  !defined(__WXGTK__) && !defined(__WXMSW__)
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWnd w;
    w.show();
    return a.exec();
    // Terminal > Set Character Encoding > Add or remove > Encodings > GB18030 Chinese Simplified
    // Terminal > Set Character Encoding > Chinese Simplified(GB18030)
    // find out list of locale on CentOS 7: locale -a | grep zh
    return 0;
}
#endif

int MainApp::LoadDict() {
	using namespace boost::iostreams;
#ifdef _MSC_VER
	HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MYDICT1), _T("MYDICT"));
	DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hRes);
	HGLOBAL hGlob = LoadResource(GetModuleHandle(NULL), hRes);
	const char* pData = reinterpret_cast<const char*>(::LockResource(hGlob));
	basic_array_source<char> input_source(pData, dwSize);
	const char* console_encoding = "zh_CN.gb18030"; // windows
#else
	extern char _binary_dict_txt_start, _binary_dict_txt_size; // objdump -t dict.bin
	basic_array_source<char> input_source(&_binary_dict_txt_start, (size_t) &_binary_dict_txt_size);
	const char* console_encoding = "GB18030"; // linux	
#endif
	stream<basic_array_source<char> > wifs(input_source);
    char linebuf[1024];	
    while(wifs.getline(linebuf,1024)) {
//    	std::string txtline = boost::locale::conv::from_utf(linebuf, console_encoding);
    	std::wstring wkey, wline;
//    	std::string nkey, nline;
//    	nkey = boost::locale::conv::between((const char*)chineseword,
//    			std::string("UTF-8"), 			// to encoding
//				std::string(console_encoding)); // from encoding
//    	wkey = convert_utf8_to_utf16(nkey);
    	wline = convert_utf8_to_utf16(linebuf);
    	dict.push_back(wline);
//    	if (wline.find(wkey)!=std::string::npos)
//    	{
//    	std::cout << txtline << std::endl;
//    	std::ios::sync_with_stdio(false);
//    	std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
//    	std::wcout << wline << std::endl;
//    	}
//    	// check if every line has exact 1 [ and ]
//    	if (std::count(wline.begin(), wline.end(), L'[')!=1 || std::count(wline.begin(), wline.end(), L']')!=1)
//    	{
//			std::cout << txtline << std::endl;
//			std::ios::sync_with_stdio(false);
//			std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
//			std::wcout << wline << std::endl;
//    	}
    }
    return 0;
}

MainApp* global_inst;
MainApp* MainApp::Instance(){
	if(!global_inst) {
		global_inst = new MainApp;
		global_inst->LoadDict();
	}
	return global_inst;
}

std::wstring MainApp::chinese_to_pinyin(const std::wstring& chinese) {
	std::wostringstream wss;
	if (dict.size()<1)
		LoadDict();
	int foundcnt=0;
#ifdef _MSC_VER
	std::locale loc("zh-CN");
#else
	std::locale loc("zh_CN.gb18030");
	//std::setlocale(LC_ALL, "zh_CN.gb18030");
#endif
	setlocale(LC_ALL, loc.name().c_str());
 
	for(auto onechar:chinese) {

		if(isspace(onechar, loc))
			continue;
#ifdef _MSC_VER
//		if (isblank(onechar))
//			continue;
#else
		//		if(isspace(onechar))
		//			continue;
		if(std::isblank(onechar))
			continue;
		if(isdigit(onechar, loc))
			continue;
#endif
		if (onechar >= L'0'&&onechar <= L'9')
			continue;
		if(onechar>=L'a'&&onechar<=L'z')
			continue;
		if(onechar>=L'A'&&onechar<=L'Z')
			continue;
		for(auto line: dict) {
			if (line.find(onechar)!=line.npos) {
				wss << onechar << line.substr(line.find(L'[')) << std::endl;
				foundcnt++;
			}
		}
	}
//	if (chinese.length()>foundcnt)
//		wss << L"input lenght is " << chinese.length() << " found " << foundcnt << std::endl;
	wss<<L"Note: this software does not understand polyphone!\n";
	wss<<L"dictionary content is from https://chinese-character-2-pinyin.googlecode.com/files/dict.rar\n";
	wss << L"You may check 3rd party online tool - http://hanyu.iciba.com/pinyin\n";
	return wss.str();
}
