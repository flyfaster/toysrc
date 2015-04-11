#pragma once
#include "mousework.h"

class DrawLine :public IMouseWork
{
public:
	DrawLine(void);
	virtual ~DrawLine(void);
	void OnLButtonDown(UINT nFlags, ExPoint point);
	void OnMouseMove(UINT nFlags, ExPoint point);
	bool IsFinished();
	void OnDraw(CdcmviewerView *pview,CDC *pdc);
private:
	ExPoint m_first;
	ExPoint m_second;
};
