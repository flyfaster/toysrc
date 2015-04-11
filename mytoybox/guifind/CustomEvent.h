#ifndef __CUSTOM_EVENT_H
#define __CUSTOM_EVENT_H

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxCUSTOM_EVENT,7777)
END_DECLARE_EVENT_TYPES()

class wxCustomEvent : public wxEvent
{
public:
	wxCustomEvent();
	virtual wxEvent *Clone() const;
//		{ return new wxCustomEvent(*this); };
//	wxEventType m_eventType;
	DECLARE_DYNAMIC_CLASS(wxCustomEvent)
};

typedef void (wxEvtHandler::*OnwxCustomEvent)(wxCustomEvent&);

#define EVT_CUSTOM_EVENT(fn)                                             \
	DECLARE_EVENT_TABLE_ENTRY( wxCUSTOM_EVENT, wxID_ANY, wxID_ANY,   \
	(wxObjectEventFunction)(wxEventFunction)(OnwxCustomEvent)&fn, (wxObject*) NULL ),

#endif