#pragma once

class CardView :
	public wxPanel
{
public:
	CardView(wxWindow* parent);
	~CardView(void);
	DECLARE_EVENT_TABLE();
	void PaintEventFunction(wxPaintEvent& evt);
	wxImage m_img;
};
