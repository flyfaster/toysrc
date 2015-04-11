#pragma once
#include <map>
#include <loki/singleton.h>
#include "guicopy_include.h"
#include "guicopy_types.h"
class Card
{
public:
	Card(int index);
	Card(void);
	~Card(void);
	static tchar_string GetName(int i);
	static int CardWidth();
	static int CardHeight();
	static int VisibleWidth();
	static int VisibleHeight();
	static bool PixelMatch( const wxImage& src, const wxImage& src2, const wxRect& src1_rect, const wxRect& src2_rect );
	static bool WhiteMatch( const wxImage& src, const wxImage& src2, const wxRect& src1_rect, const wxRect& src2_rect );
	
	enum FACE{INVALID_FACE=-1, TWO, THREEE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, J, Q, K, A, JOKER};
	enum PATTERN {INVALID_PATTERN=-1, DIAMOND, CLUB, HEART, SPADE, SMALL, BIG};
	enum {INVALID_VALUE=-1};
	bool operator< (const Card& other) const;
	bool operator==(const Card& other) const;
	int GetValue() const;
	void SetValue(int index);
	tchar_string GetName() const;
	wxRect GetRect() const;
	static bool IsValidIndex(int index);
private:
	int m_index_value;
	FACE m_face;
	PATTERN m_pattern;
};

typedef std::map<Card, int> CardSet;

class CardSuit : public CardSet
{
public:
	void Add (int index);
	void Remove(int index);

};

typedef Loki::SingletonHolder<CardSuit, Loki::CreateUsingNew, Loki::PhoenixSingleton> SingleCardSuit;