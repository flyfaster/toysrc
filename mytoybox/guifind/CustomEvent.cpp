/*
 * CustomEvent.cpp
 *
 *  Created on: 10 Apr 2010
 *      Author: onega
 */
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/gbsizer.h"
#include <wx/textctrl.h>
#include "wx/dirdlg.h"
#include <boost/thread/mutex.hpp>
#include "CustomEvent.h"

DEFINE_LOCAL_EVENT_TYPE(wxCUSTOM_EVENT);


IMPLEMENT_DYNAMIC_CLASS( wxCustomEvent, wxEvent )

wxCustomEvent::wxCustomEvent()
{
	SetId(wxID_ANY);
	SetEventType(wxCUSTOM_EVENT);
}

wxEvent *wxCustomEvent::Clone() const
{
	return new wxCustomEvent(*this);
};
