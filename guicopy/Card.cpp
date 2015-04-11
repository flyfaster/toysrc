#include <sstream>
#include <string>
#include <tchar.h>
#include <WTypes.h>
#include <wx/image.h>
#include "guicopy_include.h"
#include "Card.h"
#include <wx/wxprec.h>
#include "mainapp.h"

Card::Card(void)
{
	m_index_value = INVALID_VALUE;
	m_face = INVALID_FACE;
	m_pattern = INVALID_PATTERN;
}

Card::Card( int index )
{
	SetValue(index);
}
Card::~Card(void)
{

}

tchar_string Card::GetName( int i )
{
	if (i<0 || i>54)
		return _T("Invalid Card");
	// spade, heart, club, diamond
	LPCTSTR number[] = {_T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
						_T("10"), _T("J"), _T("Q"), _T("K"), _T("A")};
	LPCTSTR face_name[]={ _T("small joker"),_T("big joker"), _T("pass")};
	LPCTSTR pattern[] = {_T("Diamond"), _T("Club"), _T("Heart"), _T("Spade")};
	std::basic_stringstream<TCHAR> ts;
	if (i/13 < 4)
		ts << pattern[i/13] << _T(" ") << number[i%13];
	else
		ts << face_name[i%13];
	return ts.str();
}

tchar_string Card::GetName() const
{
	return GetName(m_index_value);
}

int Card::CardWidth()
{
	return 60;
}

int Card::CardHeight()
{
	return 90;
}

int Card::VisibleWidth()
{
	return 12;
}

int Card::VisibleHeight()
{
	return 40;
}

bool Card::PixelMatch( const wxImage& src, const wxImage& src2, const wxRect& src1_rect, const wxRect& src2_rect )
{
	int width=src1_rect.GetWidth();
	int height = src1_rect.GetHeight();
	int x1, y1, x2, y2;
	const int COLOR_DIFF = 9;
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			x1 = x + src1_rect.GetLeft();
			y1 = y + src1_rect.GetTop();
			x2 = x + src2_rect.GetLeft();
			y2 = y + src2_rect.GetTop();
			if( 
				abs(src.GetRed(x1, y1)-src2.GetRed(x2, y2))>COLOR_DIFF
				|| abs(src.GetGreen(x1, y1)-src2.GetGreen(x2, y2))>COLOR_DIFF
				|| abs(src.GetBlue(x1, y1)-src2.GetBlue(x2, y2))>COLOR_DIFF
				)
			{
				//std::basic_stringstream<TCHAR> ts;
				//ts << wxT("(") << x1 
				//	<< wxT(", ") << y1 << wxT(") = ")
				//	<< wxT("{") << (UINT)src.GetRed(x1, y1) << wxT(", ") << (UINT)src.GetGreen(x1, y1) << wxT(",") << (UINT)src.GetBlue(x1, y1) << wxT("} ")
				//	<< wxT("      (") << x2
				//	<< wxT(", ") << y2 << wxT(") = ")					
				//	<< wxT("{") << (UINT)src2.GetRed(x2, y2) << wxT(", ") << (UINT)src2.GetGreen(x2, y2) << wxT(",") << (UINT)src2.GetBlue(x2, y2) << wxT("} ");
				//wxGetApp().Dump(ts.str());
				return false;
			}
		}
		return true;
}

bool Card::WhiteMatch( const wxImage& src1, const wxImage& src2, const wxRect& src1_rect, const wxRect& src2_rect )
{
	const unsigned char R_VALUE = 255;
	const unsigned char G_VALUE = 255;
	const unsigned char B_VALUE = 255;
	int width=src1_rect.GetWidth();
	int height = src1_rect.GetHeight();
	int x1, y1, x2, y2;
	const int COLOR_DIFF = 9;
	bool white2 = false;
	bool white1 = false;
	int white_difference = 0;
	const int MAX_WHITE_DIFFERENT = 3;
	if ( (src1.GetRed(src1_rect.GetLeft(), src1_rect.GetTop())==8)
		&& (src1.GetGreen(src1_rect.GetLeft(), src1_rect.GetTop())==65)
		&& (src1.GetBlue(src1_rect.GetLeft(), src1_rect.GetTop())==0))
	{
		// board of card
		for (int y=0; y<height; y++)
		{
			int row_diff = 0;
			for (int x=0; x<width; x++)
			{
				x1 = x + src1_rect.GetLeft();
				y1 = y + src1_rect.GetTop();
				x2 = x + src2_rect.GetLeft();
				y2 = y + src2_rect.GetTop();
				white1 = ( 
					(src1.GetRed(x1, y1) >= R_VALUE)
					&& (src1.GetBlue(x1, y1) >= B_VALUE)
					&& (src1.GetGreen(x1, y1) >= G_VALUE)
					);
				white2 = ( 
					(src2.GetRed(x2, y2) >= R_VALUE)
					&& (src2.GetBlue(x2, y2) >= B_VALUE)
					&& (src2.GetGreen(x2, y2) >= G_VALUE)
					);
				if (white2!=white1)
				{
					row_diff++;
				}			
			}
			white_difference+=row_diff;
			if (row_diff>1)
				return false;
		}
	}
	else
	{
		// maybe pass or nothing
		// search a card size, count Blue = 132, 69, 128
		// 80, 90	88, 127, 127, 127

		//{{220,40}, {80,90}, {352,90}, {80,200}, {352,200}, {220,250}};
		if (   (src1_rect.GetLeft()!=80)
			&& (src1_rect.GetLeft()!=220)
			&& (src1_rect.GetLeft()!=352))
			return false;
		if (   (src2.GetRed(src2_rect.GetLeft(), src2_rect.GetTop())!=255)
			|| (src2.GetBlue(src2_rect.GetLeft(), src2_rect.GetTop())!=255)
			|| (src2.GetGreen(src2_rect.GetLeft(), src2_rect.GetTop())!=0)
			)
			return false;
		const unsigned char PASS_BLUE_VALUE = 132;
		int blue_count = 0;
		for (int i=0; i<=127-88; i++)
		{
			if (   (src1.GetBlue(src1_rect.GetLeft()+8+i, src1_rect.GetTop()+37)==PASS_BLUE_VALUE)
				&& (src1.GetRed(src1_rect.GetLeft()+8+i, src1_rect.GetTop()+37)> 200)
				&& (src1.GetGreen(src1_rect.GetLeft()+8+i, src1_rect.GetTop()+37)> 200))
			{
				blue_count++;
			}
		}
		tstringstream ts;
		ts << _T("blue count is ") << blue_count;
		wxLogVerbose(ts.str().c_str());
		if (blue_count>5) // magic number
			return true; // pass index
		else
			return false;
	}
	//if (white_difference>MAX_WHITE_DIFFERENT)
	//	return false;
	return true;
}

bool Card::operator<( const Card& other ) const 
{
	return (m_face*14+m_pattern) < (other.m_face*14+other.m_pattern);
	//return m_index_value < other.
}

bool Card::operator ==(const Card &other) const
{
	//return (m_face==other.m_face) && (m_pattern==other.m_pattern);
	return m_index_value == other.m_index_value;
}

void Card::SetValue( int index )
{
	if (index<0 || index>=54)
	{
		std::stringstream ss;
		ss << __FUNCTION__ << "(" << index << ")";
		throw std::invalid_argument(ss.str());
	}
	m_index_value = index;
	if ((m_index_value/13) < 4)
	{
		m_face = (FACE)(m_index_value%13);
		m_pattern = (PATTERN)(m_index_value/13);
	}
	else
		{
			m_face = JOKER;
			m_pattern=(PATTERN)(m_index_value%13+SMALL);
		}
}

int Card::GetValue() const
{
	return m_index_value;
}

wxRect Card::GetRect() const
{
	wxRect ret;
	int row = m_index_value/13;
	int column = m_index_value %13;
	ret.SetLeft(CardWidth()*column);
	ret.SetTop(CardHeight()*row);
	ret.SetWidth(CardWidth());
	ret.SetHeight(CardHeight());
	if (m_face==THREEE)
	{
		tstringstream ts;
		ts << ret.GetLeft() << _T(",") << ret.GetTop() << _T(",") 
			<< ret.GetWidth() << _T(",") << ret.GetHeight();
		wxLogVerbose(ts.str().c_str());
	}
	return ret;
}

bool Card::IsValidIndex( int index )
{
	return  ((index>=0) && (index<54));
}
void CardSuit::Add( int index )
{
	if (!Card::IsValidIndex(index))
		return;
	Card tmp(index);
	CardSet::iterator it = find(tmp);
	if ( it!=this->end())
		it->second += 1;
	else
		this->insert(std::make_pair(tmp, 1));
}

void CardSuit::Remove( int index )
{
	if (!Card::IsValidIndex(index))
		return;
	CardSet::iterator it= find(Card(index));
	if (it!=end())
	{
		it->second -= 1;

	}
}