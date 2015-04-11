#pragma once

class COMInitializer
{
public:
	COMInitializer(DWORD dwCoInit = COINIT_APARTMENTTHREADED);
	~COMInitializer();
	bool Error();
	operator HRESULT() const
	{
		return m_hr;
	}
	DWORD GetThreadId();
private:
	HRESULT m_hr;
	DWORD m_init_thread;
};
std::basic_string<TCHAR> ToString( VARTYPE vt );