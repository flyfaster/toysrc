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

#ifndef __WXGTK__
#include "mainwnd.h"
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

using namespace std;    // Sorry for this!
//QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));

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


void read_all_lines(const char *filename, const char* chineseword)
{
    char linebuf[1024];
    std::ifstream wifs(filename);
    const char* console_encoding = "GB18030";
    while(wifs.getline(linebuf,1024)) {
    	string txtline = boost::locale::conv::from_utf(linebuf, console_encoding);
    	std::wstring wkey, wline;
    	std::string nkey, nline;
    	nkey = boost::locale::conv::between((const char*)chineseword,
    			std::string("UTF-8"), 			// to encoding
				std::string(console_encoding)); // from encoding
    	wkey = convert_utf8_to_utf16(nkey);
    	wline = convert_utf8_to_utf16(linebuf);
    	if (wline.find(wkey)!=std::string::npos)
    	{
    	std::cout << txtline << std::endl;
    	std::ios::sync_with_stdio(false);
    	std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
    	std::wcout << wline << std::endl;
//    	break;
    	}
    	// check if every line has exact 1 [ and ]
    	if (std::count(wline.begin(), wline.end(), L'[')!=1 || std::count(wline.begin(), wline.end(), L']')!=1)
    	{
			std::cout << txtline << std::endl;
			std::ios::sync_with_stdio(false);
			std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
			std::wcout << wline << std::endl;
    	}
    }
}

#ifndef __WXGTK__
int main(int argc, char* argv[])
{

    QApplication a(argc, argv);
    MainWnd w;
    w.show();
    return a.exec();

	// Console output will be UTF-16 characters
    if(argc < 2)
    {
        wcerr << L"Filename expected!" << endl;
        return 1;
    }
    // Terminal > Set Character Encoding > Add or remove > Encodings > GB18030 Chinese Simplified
    // Terminal > Set Character Encoding > Chinese Simplified(GB18030)
    // find out list of locale on CentOS 7: locale -a | grep zh
    read_all_lines(argv[1], argv[2]);
    return 0;
}
#endif

int MainApp::LoadDict() {
	extern char _binary_dict_txt_start, _binary_dict_txt_size; // objdump -t dict.bin
	using namespace boost::iostreams;
	basic_array_source<char> input_source(&_binary_dict_txt_start, (size_t) &_binary_dict_txt_size);
	stream<basic_array_source<char> > wifs(input_source);
    char linebuf[1024];
//    std::ifstream wifs("dict.txt");
    const char* console_encoding = "GB18030";
    while(wifs.getline(linebuf,1024)) {
    	std::string txtline = boost::locale::conv::from_utf(linebuf, console_encoding);
    	std::wstring wkey, wline;
    	std::string nkey, nline;
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
	for(auto onechar:chinese) {
//		if(isspace(onechar))
//			continue;
		if(isblank(onechar))
			continue;
		if(isdigit(onechar))
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
	return wss.str();
}
