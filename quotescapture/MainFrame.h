#pragma once
//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
#include <loki/pimpl.h>
//#include <loki/Singleton.h>

class STOCKCLIENT_API MainFrame 
#ifdef USE_LOKI_PIMPL
	: private Loki::PimplOf<MainFrame>::Owner
#endif
	{
	public:
		MainFrame(void);
		virtual ~MainFrame(void);
		class wxFrame* GetFrame();
		static MainFrame& Instance();
#ifndef USE_LOKI_PIMPL
		struct PrivateMainFrame* d;
#endif
	};

