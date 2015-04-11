#pragma once
#include "wx\frame.h"

class SetEventFrame :
	public wxFrame
{
public:
	SetEventFrame(void);
	~SetEventFrame(void);
	void OnCloseWindow(wxCloseEvent& event);
private:
    class wxNotebook *m_book;
    class EventPage* m_event_page;
    class OptionsPage* m_opt_page;
    class ErrorPage* m_error_page;
	DECLARE_EVENT_TABLE()

};
