#include "guicopy_include.h"
#include "Base64Page.h"
#include "base64.hpp"
#include <vector>
#include <sstream>
#include <wx/log.h>
#include <wx/clipbrd.h>
//#include "base64.h"

template<typename Ch, typename Traits = std::char_traits<Ch>,
typename Sequence = std::vector<Ch> >
struct basic_seqbuf : std::basic_streambuf<Ch, Traits> {
	typedef std::basic_streambuf<Ch, Traits> base_type;
	typedef typename base_type::int_type int_type;
	typedef typename base_type::traits_type traits_type;

	virtual int_type overflow(int_type ch) {
		if(traits_type::eq_int_type(ch, traits_type::eof()))
			return traits_type::eof();
		c.push_back(traits_type::to_char_type(ch));
		return ch;
	}

	Sequence const& get_sequence() const {
		return c;
	}
	void put_eof()
	{
		c.push_back(0);
	}
protected:
	Sequence c;
};

BEGIN_EVENT_TABLE(Base64Page, wxPanel)
EVT_BUTTON(wxID_ANY, Base64Page::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<Base64Page> : public wxEvtHandler
{
	wxTextCtrl* m_src;
	wxTextCtrl* m_result;
	wxButton* m_encode;
	wxButton* m_decode;
	wxButton* m_swap;
	wxButton* m_paste;
	wxComboBox* m_display_type;
	wxComboBox* m_numeric_type;
	void Encode();
	void Decode();
	void CreateControlls(wxWindow* parent);
	void OnComboboxSelected( wxCommandEvent &event );
	void OnSwap( wxCommandEvent& ev );
	void OnPaste(wxCommandEvent& ev);

};

void Loki::ImplOf<Base64Page>::OnPaste( wxCommandEvent& ev )
{
	bool ret = wxOpenClipboard();
	if (wxIsClipboardFormatAvailable(wxDF_TEXT))
	{
		long len = 0;
		char* data = (char*)wxGetClipboardData(wxDF_TEXT, &len);
		if (data && len >0)
		{
			m_src->SetValue(wxString::FromAscii(data));
		}
		else
		{
			wxLogError(wxT("%s clipboard data:0x%08x, len:%d."), data, len);
		}
	}
	ret = wxCloseClipboard();
}
void Loki::ImplOf<Base64Page>::OnSwap( wxCommandEvent& ev )
{
	wxString src = m_src->GetValue();
	m_src->SetValue(m_result->GetValue());
	m_result->SetValue(src);
}

typedef Loki::ImplOf<Base64Page> PimplBase64Page;
void Loki::ImplOf<Base64Page>::CreateControlls( wxWindow* parent )
{
	m_src = new wxTextCtrl(parent, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	m_result = new wxTextCtrl(parent, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	m_encode = new wxButton(parent, wxID_ANY, _T("Base64 Encode"));
	m_decode = new wxButton(parent, wxID_ANY, _T("Base64 Decode"));
	m_swap = new wxButton(parent, wxID_ANY, _T("Swap"));
	m_paste = new wxButton(parent, wxID_ANY, _T("Paste"));
	m_display_type = new wxComboBox(parent, wxID_ANY);
	m_display_type->AppendString(_T("Text"));
	m_display_type->AppendString(_T("Binary"));
	m_display_type->AppendString(_T("Mixed"));
	m_numeric_type = new wxComboBox(parent, wxID_ANY);
	m_numeric_type->AppendString(_T("BYTE"));
	m_numeric_type->AppendString(_T("SHORT"));
	m_numeric_type->AppendString(_T("DWORD"));
	m_display_type->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, 
		wxCommandEventHandler(PimplBase64Page::OnComboboxSelected), NULL, this);
	m_display_type->Select(0);
	m_swap->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PimplBase64Page::OnSwap), NULL, this);
	m_paste->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PimplBase64Page::OnPaste), NULL, this);
}

void Loki::ImplOf<Base64Page>::Decode()
{
	//wxLogVerbose( wxString::FromAscii(__FUNCTION__));
	wxString src = m_src->GetValue();
	int _State = 0;
	base64<TCHAR> encoder;

	typedef basic_seqbuf<TCHAR> seqbuf;
	seqbuf decode_buf;

	std::ostreambuf_iterator<TCHAR> _Out(&decode_buf);
	encoder.get(src.wx_str(), src.wx_str()+src.length(), _Out, _State);

	if (decode_buf.get_sequence().size()==0)
	{
		wxLogWarning(wxT("%s empty output."), wxString::FromAscii(__FUNCTION__));
		return;
	}
	wxString display_type =  m_display_type->GetValue();
	wxString numeric_type = m_numeric_type->GetValue();
	if (display_type == _T("Text"))
	{
		decode_buf.put_eof();
		m_result->SetValue( decode_buf.get_sequence().begin().operator->() );
		return;
	}
	tstringstream tss;
    typedef std::vector<TCHAR> tchar_vector;
	if (display_type == _T("Mixed"))
	{
		int pos = 0;
		tchar_vector const& decode_result = decode_buf.get_sequence();
		tss << std::hex;
		for (tchar_vector::const_iterator it=decode_result.begin();
			it!=decode_result.end();
			it++)
		{
			unsigned long data = *it;
			if (isprint(*it) || isspace(*it))
			{
				tss << *it;
			}
			else
			{
				tss <<_T(" ")<< std::setw(2) << std::setfill(wxT('0')) << data << _T(" ");
				pos++;
			}
			if ((pos % 32) == 0)
				tss << std::endl;
		}
		m_result->SetValue(tss.str().c_str());
		return;
	}
	if (display_type == _T("Binary"))
	{
		int pos = 0;
		tchar_vector const& decode_result = decode_buf.get_sequence();
		if (numeric_type == _T("BYTE"))
		{
			tss << std::hex;
			for (tchar_vector::const_iterator it=decode_result.begin();
				it!=decode_result.end();
				it++)
			{
				tss << std::setw(2) << std::setfill(wxT('0')) << (((unsigned long)*it)&0xff);
				pos++;
				if ((pos % 32) == 0)
					tss << std::endl;
				else 
					tss << _T(" ");
			}
			m_result->SetValue(tss.str().c_str());
			return;
		}
		if (numeric_type == _T("SHORT"))
		{
			tss << std::hex;
			if ((decode_result.size()%2) !=0)
			{
				wxLogError(_T("%s size is %d."), wxString::FromAscii(__FUNCTION__), decode_result.size());
				return;
			}
			for (tchar_vector::const_iterator it=decode_result.begin();
				it!=decode_result.end();
				)
			{
				unsigned long c1, c2;
				c1 = *it;
				it++;
				c2 = *it;
				it++;
				tss << std::setw(2) << std::setfill(_T('0')) << c2 << std::setw(2) 
                    << std::setfill(_T('0')) << c1;
				pos++;
				if ((pos % 32) == 0)
					tss << std::endl;
				else 
					tss << _T(" ");
			}
			m_result->SetValue(tss.str().c_str());
			return;
		}
		if (numeric_type == _T("DWORD"))
		{
			tss << std::hex;
			if ((decode_result.size()%4) !=0)
			{
				wxLogError(_T("%s size is %d."), wxString::FromAscii(__FUNCTION__), decode_result.size());
				return;
			}
			for (tchar_vector::const_iterator it=decode_result.begin();
				it!=decode_result.end();
				)
			{
				unsigned long c1, c2, c3, c4;
				c1 = *it;
				it++;
				c2 = *it;
				it++;
				c3 = *it;
				it++;
				c4 = *it;
				it++;
				tss << std::setw(2) << std::setfill(_T('0')) << c4 << std::setw(2) 
                    << std::setfill(_T('0')) << c3;
				tss << std::setw(2) << std::setfill(_T('0')) << c2 << std::setw(2) 
                    << std::setfill(_T('0')) << c1;
				pos++;
				if ((pos % 16) == 0)
					tss << std::endl;
				else 
					tss << _T(" ";);
			}
			m_result->SetValue(tss.str().c_str());
			return;
		}
		return;
	}

}
void Loki::ImplOf<Base64Page>::Encode()
{
	wxLogVerbose( wxString::FromAscii(__FUNCTION__));
	wxString src = m_src->GetValue();
	int _State = 0;
	base64<TCHAR> encoder;
	tstringstream ss;
	std::ostreambuf_iterator<TCHAR> _Out(ss.rdbuf());
	encoder.put(src.wx_str(), src.wx_str()+src.length(), _Out, _State, base64<>::crlf());
	m_result->SetValue(ss.str());

}

void Loki::ImplOf<Base64Page>::OnComboboxSelected( wxCommandEvent &event )
{
	m_numeric_type->Enable(m_display_type->GetSelection()==1);
	wxLogVerbose(wxString::FromAscii(__FUNCTION__));
}

Base64Page::Base64Page(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	d->CreateControlls(this);

	int row = 0;
	int col = 0;
	wxGridBagSizer* sizer = new wxGridBagSizer();
	row=1;
	sizer->Add(d->m_paste, wxGBPosition(row, col), wxDefaultSpan, wxALIGN_RIGHT);
	col++;
	sizer->Add(d->m_encode, wxGBPosition(row, col), wxDefaultSpan, wxALIGN_RIGHT);
	col++;
	sizer->Add(d->m_display_type, wxGBPosition(row, col));
	col++;
	sizer->Add(d->m_numeric_type, wxGBPosition(row, col));
	col++;
	sizer->Add(d->m_decode, wxGBPosition(row, col));
	col++;
	sizer->Add(d->m_swap, wxGBPosition(row, col));	
	row++;
	//col = 0;
	sizer->Add(d->m_src, wxGBPosition(0, 0), wxGBSpan(1, col+1), wxALL | wxEXPAND);
	sizer->Add(d->m_result, wxGBPosition(2, 0), wxGBSpan(1, col+1), wxALL | wxEXPAND);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableCol(col);
	sizer->AddGrowableRow(0);
	sizer->AddGrowableRow(2);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

Base64Page::~Base64Page(void)
{
}

void Base64Page::OnButton( wxCommandEvent& ev )
{
	wxButton* active_button = dynamic_cast<wxButton*>(ev.GetEventObject());
	if (active_button)
	{
		if (active_button->GetLabelText()==wxT("Cancel"))
		{
			d->m_src->SetValue(wxEmptyString);
			d->m_result->SetValue(wxEmptyString);
		}
		if (active_button==d->m_encode)
		{
			d->Encode();
		}
		if (active_button==d->m_decode)
		{
			d->Decode();
		}
	}
}