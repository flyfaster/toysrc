// ProcessIterator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <algorithm>

#include "toolhelp_iterator.h"


//This is a function that does something with a THREADENTRY32
void MyThreadEntry32Proc(const THREADENTRY32& te32)
{
	std::cout << '\t' << '\t' << te32.th32ThreadID << std::endl;
}

DWORD _stdcall Go(PVOID)
{
	Sleep(10000);
	return 0;
}

int main(int argc, char* argv[])
{
	//This is a dummy thread just so we have more that one to show
	HANDLE h = CreateThread(0, 0, Go, 0, 0, 0);

	//This loop iterates over every process 
	for(CProcessIterator i(0); i!=CProcessIterator::End; i++)
	{
		std::cout << "pid=" << i->th32ProcessID << std::endl;

		CModuleIterator j(i->th32ProcessID);
		CModuleIterator module_end;

		//This loop iterates over every module of the process
		//we are iterating over (that's i.th32ProcessID)
		while(j != module_end)
		{
			std::cout << '\t' << j->szExePath << std::endl;

			++j;
		}

		//This loop iterates over every thread of the process
		//we are iterating over (that's i.th32ProcessID)
        CThreadIterator k(i->th32ProcessID, new CSingleProcessThreadIteratorFilter(i->th32ProcessID));

        while(k != CThreadIterator::End)
        {
            MyThreadEntry32Proc(*k);
            ++k;
        }
	}

	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);

	return 0;
}
