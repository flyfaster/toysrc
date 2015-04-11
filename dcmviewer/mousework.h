#pragma once
#include <Windows.h>
class ExPoint
{
public:
	virtual ~ExPoint()
	{
		init();
	}
	ExPoint()
	{
		init();
	}
	ExPoint(int x, int y,int mapmode,float angle)
	{
		m_valid = true;
	}
	ExPoint(ExPoint& d)
	{
		m_valid = true;
		m_x = d.m_x;
		m_y = d.m_y;
		m_mapmode = d.m_mapmode;
		m_angle = d.m_angle;
	}
	operator=(ExPoint& d)
	{
		m_valid = true;
		m_x = d.m_x;
		m_y = d.m_y;
		m_mapmode = d.m_mapmode;
		m_angle = d.m_angle;
	}
	bool IsValid()
	{
		return m_valid;
	}
	operator POINT()
	{
		POINT pt;
		pt.x = m_x;
		pt.y = m_y;
		return pt;
	}
private:
	void init()
	{
		m_x = 0;
		m_y = 0;
		m_mapmode = MM_TEXT;
		m_angle = 0;
		m_valid = false;

	}
	int m_x;
	int m_y;
	int m_mapmode;
	float m_angle;
	bool m_valid;
};
class CdcmviewerView;
interface IMouseWork
{
public:
	virtual void OnMouseMove(UINT nFlags, ExPoint point){}
	virtual void OnRButtonDown(UINT nFlags, ExPoint point){}
	virtual void OnLButtonDown(UINT nFlags, ExPoint point){}
	virtual bool IsFinished(){return true;	}
	virtual void OnDraw(CdcmviewerView *pview,CDC *pdc){}
};

