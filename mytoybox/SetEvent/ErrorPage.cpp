#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/clipbrd.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <boost/lexical_cast.hpp>
#include "ErrorPage.h"
#include "SetEventApp.h"

BEGIN_EVENT_TABLE(ErrorPage, wxPanel)
EVT_BUTTON(wxID_ANY, ErrorPage::OnButton)
EVT_RADIOBUTTON(wxID_ANY, ErrorPage::OnRadioButton)
END_EVENT_TABLE()


// http://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
typedef unsigned int    uint32;
typedef signed int      int32;

class uint32_from_hex   // For use with boost::lexical_cast
{
    uint32 value;
public:
    operator uint32() const { return value; }
    friend std::basic_istream<TCHAR>& operator>>( std::basic_istream<TCHAR>& in, uint32_from_hex& outValue )
    {
        in >> std::hex >> outValue.value;
        return in;
    }
};

class int32_from_hex   // For use with boost::lexical_cast
{
    uint32 value;
public:
    operator int32() const { return static_cast<int32>( value ); }
    friend std::basic_istream<TCHAR>& operator>>( std::basic_istream<TCHAR>& in, int32_from_hex& outValue )
    {
        in >> std::hex >> outValue.value;
        return in;
    }
};
//uint32 material0 = lexical_cast<uint32_from_hex>( "0x4ad" );
//uint32 material1 = lexical_cast<uint32_from_hex>( "4ad" );
//uint32 material2 = lexical_cast<uint32>( "1197" );
//
//int32 materialX = lexical_cast<int32_from_hex>( "0xfffefffe" );
//int32 materialY = lexical_cast<int32_from_hex>( "fffefffe" );

ErrorPage::ErrorPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
    m_error_code = new wxTextCtrl(this, wxID_ANY);
    m_find_btn = new wxButton(this, wxID_ANY, _T("Find"));
    m_copy_btn = new wxButton(this, wxID_ANY, _T("Copy"));
    m_clear_btn = new wxButton(this, wxID_ANY, _T("Clear"));
    m_description = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );

    m_dec = new wxRadioButton(this, wxID_ANY, wxT("Decimal error code"));
    m_hex = new wxRadioButton(this, wxID_ANY, wxT("Hex error code"));
    m_hex->SetValue(true);
    m_dec->SetValue(false);

    wxGridBagSizer* sizer=new wxGridBagSizer();
    int row = 0;
    int col = 0;
    sizer->Add(m_hex, wxGBPosition(row, col));
    col++;
    sizer->Add(m_dec, wxGBPosition(row, col));
    col++;
    sizer->Add(m_error_code, wxGBPosition(row, col));

    row++; col=0;
    sizer->Add(m_description, wxGBPosition(row, col), wxGBSpan(1, 3), wxEXPAND|wxALL);
    sizer->AddGrowableCol(0);
    sizer->AddGrowableCol(1);
    sizer->AddGrowableCol(2);
    sizer->AddGrowableRow(row);
    row++; col=0;
    sizer->Add(m_find_btn, wxGBPosition(row, col));
    col++;
    sizer->Add(m_copy_btn, wxGBPosition(row, col));
    col++;
    sizer->Add(m_clear_btn, wxGBPosition(row, col));

    SetSizer(sizer);
    sizer->SetSizeHints(this);

}

ErrorPage::~ErrorPage(void)
{
}

void ErrorPage::OnButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_find_btn)
    {
        int ec = 0;
        if (m_dec->GetValue())
            ec = _ttoi(m_error_code->GetValue());
        else
            ec = boost::lexical_cast<uint32_from_hex>( m_error_code->GetValue().wx_str() );;
        m_description->SetValue(wxGetApp().SystemErrorToString(ec));
    }
    if (event.GetEventObject()==m_clear_btn)
    {
        m_error_code->SetValue(wxEmptyString);
        m_description->SetValue(wxEmptyString);
    }
    if (event.GetEventObject()==m_copy_btn)
    {
        if (wxTheClipboard->Open())
        {
            // This data objects are held by the clipboard,
            // so do not delete them in the app.
            wxTheClipboard->SetData( new wxTextDataObject(m_description->GetValue()) );
            wxTheClipboard->Close();
        }
    }
}

void ErrorPage::OnRadioButton( wxCommandEvent& event )
{
    if (event.GetEventObject()==m_dec)
    {
        m_dec->SetValue(true);
        m_hex->SetValue(false);
    }
    else
    {
        m_dec->SetValue(false);
        m_hex->SetValue(true);
    }
}
