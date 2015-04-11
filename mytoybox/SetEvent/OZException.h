#pragma once
class OZException :
	public std::exception
{
public:
	typedef std::basic_string<TCHAR> tstring;
	typedef std::basic_stringstream<TCHAR> tstringstream;
	OZException(LPCSTR function, int line, LPCTSTR description, int error_code);
	~OZException(void);
	const char* what();
	tstring ToString() const;
	int line;
	int error_code;
	tstring function_name;
	tstring description;
	static void CheckHRESULT(LPCSTR function, int line, LPCTSTR description, HRESULT error_code);
    template<typename T>
    static void NullException(LPCSTR function, int line, LPCTSTR description, const T* data_pointer)
    {
        if (NULL==data_pointer)
            throw OZException(function, line, description, E_FAIL);
    }
};

