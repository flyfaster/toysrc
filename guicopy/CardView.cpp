#include <wx/panel.h>
#include <wx/image.h>
#include <wx/imagbmp.h>
#include <wx/dcmemory.h>
 
#include <boost/scoped_ptr.hpp>
#include "CardView.h"
#include "Card.h"

BEGIN_EVENT_TABLE(CardView, wxPanel)
EVT_PAINT(CardView::PaintEventFunction)
END_EVENT_TABLE()

CardView::CardView(wxWindow* parent):wxPanel(parent,wxID_ANY )
{
	m_img.LoadFile(wxT("E:\\svn\\guicopy\\dalu\\bmp184.bmp"));
}

CardView::~CardView(void)
{
}

void CardView::PaintEventFunction( wxPaintEvent& evt )
{
	wxPaintDC dc(this);
	wxMemoryDC mem_dc;
	int w,h;
	GetClientSize(&w, &h);
	wxBitmap bmp (w, h);	
	mem_dc.SetBackground(*wxBLACK_BRUSH);
	mem_dc.Clear();
	mem_dc.SelectObject(bmp);
	
	int card_number=0;
	const int CARDS_DISPLAY_ROW_SIZE = 54;
	
	for (CardSet::iterator it=SingleCardSuit::Instance().begin();
		it!=SingleCardSuit::Instance().end();
		it++)
	{
		wxImage onecard = m_img.GetSubImage(it->first.GetRect());
		wxBitmap one_card_bmp(onecard);
		for (int i=0; i<it->second; i++)
		{
			wxPoint current_card_position(Card::VisibleWidth()*(card_number%CARDS_DISPLAY_ROW_SIZE), 
				(card_number/CARDS_DISPLAY_ROW_SIZE)*(Card::VisibleHeight()));
			card_number++;
			mem_dc.DrawBitmap(one_card_bmp, current_card_position, false);
		}
	}
	dc.Blit(0,0, w, h, &mem_dc, 0,0);
}