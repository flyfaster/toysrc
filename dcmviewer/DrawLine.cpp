#include "StdAfx.h"
#include ".\drawline.h"

DrawLine::DrawLine(void)
{
}

DrawLine::~DrawLine(void)
{
}

void DrawLine::OnLButtonDown(UINT nFlags, ExPoint point)
{

}

void DrawLine::OnMouseMove(UINT nFlags,ExPoint point)
{

}

bool DrawLine::IsFinished()
{
	return m_first.IsValid()&&m_second.IsValid();
}

void DrawLine::OnDraw(CdcmviewerView *pview,CDC *pdc)
{
	if(NULL==pview||NULL==pdc)
		return;
	if(IsFinished())
	{
		pdc->MoveTo(m_first);
		pdc->LineTo(m_second);
	}
}