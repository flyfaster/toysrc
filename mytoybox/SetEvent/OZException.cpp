#include <string>
#include <sstream>
#include <tchar.h>
#include <exception>
#include <iomanip>
#include <windows.h>
#include "OZException.h"

OZException::OZException(LPCSTR function, int line, LPCTSTR description, int error_code)
{
	if (function)
	{
		std::string tmp = function;
		function_name.resize(tmp.length(), _T(' '));
		std::copy(tmp.begin(), tmp.end(), function_name.begin());
	}
	if (description)
		this->description = description;
	this->line = line;
	this->error_code = error_code;
}

OZException::~OZException(void)
{
}

OZException::tstring OZException::ToString() const
{
	tstringstream ts;
	ts << _T("Exception in ") << function_name
		<< _T(" line ") << line
		<< _T(", code ") << std::hex <<std::showbase<<std::internal<< error_code
		<< _T(", description ") << description;
	return ts.str();
}

void OZException::CheckHRESULT( LPCSTR function, int line, LPCTSTR description, HRESULT error_code )
{
	if (FAILED(error_code))
		throw OZException(function, line, description, error_code);
}
