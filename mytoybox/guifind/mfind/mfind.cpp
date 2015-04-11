// mfind.cpp : Defines the entry point for the console application.
//
// Usage: mfind.exe <file name> <pattern 1> <pattern 2> ...
// Build command: cl /MT /D "_UNICODE" /EHsc mfind.cpp

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_ifstream<TCHAR> tfstream;

#ifdef _UNICODE
std::basic_ostream<TCHAR> &tcout = std::wcout;
#else 
std::basic_ostream<TCHAR> &tcout = std::cout;
#endif

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc<3)
    {
        tcout << _T("Usage: ") << argv[0] << _T(" <file name> <pattern 1> <pattern 2> ...") << std::endl;
        return __LINE__;
    }
    tfstream input;
    input.open(argv[1]);
    if (input.bad())
    {
        tcout << argv[1] << _T(" failed to open ") << argv[1] << std::endl;
        return __LINE__;
    }
    tstring stmp;
    while (input.good())
    {
        std::getline(input, stmp);
        for (int i=2; i< argc; i++)
        {
            if (stmp.find(argv[i])!=stmp.npos)
			{
                tcout << stmp << std::endl;
				break;
			}
        }
    }
	return 0;
}

