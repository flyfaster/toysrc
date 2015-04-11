#pragma once

class ScopeTracer
{
public:

	ScopeTracer(LPCTSTR title)
	{
		if (title)
			m_title = title;
		m_start_time = GetTickCount();
	}

	~ScopeTracer(void)
	{
		DWORD elapsed_time = GetTickCount() - m_start_time;
		std::basic_stringstream<TCHAR> ss;
		ss << m_title << " took " << elapsed_time << " milliseconds.";
		if (IsDebuggerPresent())
			ss << std::endl;
		OutputDebugString(ss.str().c_str());

	}
private:
	DWORD m_start_time;
	std::basic_string<TCHAR> m_title;
};
