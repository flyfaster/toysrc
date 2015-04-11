#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/Panel.h>
#include <wx/clipbrd.h>
#include <wx/confbase.h>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iomanip>
#include <map>
#include <atlbase.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <rpcdce.h>
#include <wincred.h>
#include <strsafe.h>
#include <loki/Pimpl.h>
#include "RegistryPage.h"
#include "OZException.h"
#include "wmihelper.h"
#include "StdRegProv.h"

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;
LPCTSTR REGISTRY_PAGE_MACHINE = wxT("Machine");

BEGIN_EVENT_TABLE(RegistryPage, wxPanel)
	EVT_BUTTON(wxID_ANY, RegistryPage::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<RegistryPage> : public wxEvtHandler
{
	wxButton *m_machine_btn;
	wxButton *m_get_btn;
	wxButton *m_set_btn;
	wxButton *m_clear_log;
	wxTextCtrl* m_machine_input;
	wxStaticText* m_user_label;
	wxStaticText* m_pwd_label;
	wxTextCtrl* m_user_input;
	wxTextCtrl* m_pwd_input;
	wxTextCtrl* m_reg_input;
	wxTextCtrl* m_verbose_output;
	void CreateUI(wxWindow* parent);
	void OnReadRegistry( wxCommandEvent& event );
	void OnWriteRegistry( wxCommandEvent& event );
	void OnClearLog(wxCommandEvent& evt);
	void DisplayDuration( LPCSTR name, wxDateTime& start_time )
	{
		wxString msg;
		wxTimeSpan duration = wxDateTime::Now() - start_time;
		if (duration.GetSeconds()>4)
		{
			msg.Printf(_T("%s spends %d seconds\n"), wxString::FromAscii(name), duration.GetSeconds().ToLong());
		}
		else
		{
			msg.Printf(_T("%s spends %d milliseconds\n"), wxString::FromAscii(name), duration.GetMilliseconds().ToLong());
		}
		m_verbose_output->AppendText(msg);
	}
	void Log(LPCTSTR msg)
	{
		if (!msg)
			return;
		tstring tname = (LPCTSTR)msg;
		boost::algorithm::replace_all(tname, _T("\n"), _T("\r\n"));
		if (boost::algorithm::ends_with(tname, _T("\n"))==false)
			tname.append(_T("\r\n"));
		m_verbose_output->AppendText(tname.c_str());
	}
};

void Loki::ImplOf<RegistryPage>::CreateUI(wxWindow* parent)
{		
	m_machine_btn = new wxButton(parent, wxID_ANY, wxT("Machine..."));
	m_get_btn = new wxButton(parent, wxID_ANY, wxT("Read"));
	m_set_btn = new wxButton(parent, wxID_ANY, wxT("Write"));
	m_clear_log = new wxButton(parent, wxID_ANY, wxT("Clear log"));
	m_get_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Loki::ImplOf<RegistryPage>::OnReadRegistry), NULL, this);
	m_set_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Loki::ImplOf<RegistryPage>::OnWriteRegistry), NULL, this);
	m_clear_log->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Loki::ImplOf<RegistryPage>::OnClearLog), NULL, this);

	m_machine_input = new wxTextCtrl(parent, wxID_ANY, _T("."));
	m_user_input = new wxTextCtrl(parent, wxID_ANY);
	m_pwd_input = new wxTextCtrl(parent, wxID_ANY);
	m_user_label = new wxStaticText(parent, wxID_ANY, wxT("User"));
	m_pwd_label = new wxStaticText(parent, wxID_ANY, wxT("Pwd"));
	m_reg_input = new wxTextCtrl(parent, wxID_ANY, _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
	m_verbose_output = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE|wxVSCROLL|wxSIMPLE_BORDER );
	wxGridBagSizer* sizer = new wxGridBagSizer();
	int col = 0;
	int row = 0;
	sizer->Add(m_machine_btn, wxGBPosition(row, col));
	col++;
	sizer->Add(m_machine_input, wxGBPosition(row, col));
	col++;
	sizer->Add(m_user_label, wxGBPosition(row, col));
	col++;
	sizer->Add(m_user_input, wxGBPosition(row, col));
	col++;
	sizer->Add(m_pwd_label, wxGBPosition(row, col));
	col++;
	sizer->Add(m_pwd_input, wxGBPosition(row, col));
	col=0;
	row++;
	sizer->Add(new wxStaticText(parent, wxID_ANY,_T("Registry Values")), wxGBPosition(row, col));
	row++;
	int col_span = sizer->GetCols()+1;
	sizer->Add(m_reg_input, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL|wxVERTICAL);
	sizer->AddGrowableRow(row);
	row++;
	wxBoxSizer* boxsizer = new wxBoxSizer(wxHORIZONTAL);
	boxsizer->Add(m_get_btn);
	boxsizer->AddSpacer(2);
	boxsizer->Add(m_set_btn);
	boxsizer->AddSpacer(2);
	boxsizer->Add(m_clear_log);
	sizer->Add(boxsizer, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND|wxALL);
	row++;
	sizer->Add(new wxStaticText(parent, wxID_ANY,_T("Verbose log")), wxGBPosition(row, col));
	row++;
	sizer->Add(m_verbose_output, wxGBPosition(row, col), wxGBSpan(1, col_span), wxEXPAND | wxHORIZONTAL|wxVERTICAL);
	sizer->AddGrowableRow(row);
	sizer->AddGrowableCol(sizer->GetCols()-1);
	parent->SetSizer(sizer);
	sizer->SetSizeHints(parent);	
	m_machine_input->SetValue(wxConfigBase::Get()->Read(REGISTRY_PAGE_MACHINE, wxT(".")));
}

void Loki::ImplOf<RegistryPage>::OnReadRegistry( wxCommandEvent& event )
{
	m_verbose_output->AppendText(wxString(__FUNCTION__));
	if (m_machine_input->GetValue().length()<1)
	{
		m_machine_input->SetValue(_T("."));
	}
	if (m_reg_input->GetValue().Length()<1)
	{
		Log(_T("Need to enter registry path.\n"));
		return;
	}
	wxDateTime m_start_time = wxDateTime::Now();
	wxString wmi_namespace = wxString::Format(wxT("\\\\%s\\root\\default"), m_machine_input->GetValue());
	tstringstream tss;
	try 
	{
		wmihelper wmisvc;
		HRESULT hr = wmisvc.Connect( 
			wmi_namespace,
			m_user_input->GetValue(),
			m_pwd_input->GetValue());				
		tss << _T("\\\\") << m_machine_input->GetValue() << _T("\\root\\default:StdRegProv");
		tstring objectpath = tss.str();
		CStdRegProv regprov(wmisvc.m_Services, objectpath);
		tss << std::endl;
		unsigned int uValue=0;
		unsigned int ret = regprov.GetDWORDValue(
			(LPCTSTR)m_reg_input->GetValue(), //_T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\InstallDate"), 
			uValue);
		tss << _T("ReturnValue is ") << ret << _T(", registry value ") << uValue << std::endl;
	}
	catch (const OZException& e)
	{
		tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< e.ToString() << std::endl;
		tstring	csErrRef;
		wmihelper::GetLastErrorWMI(csErrRef, e.error_code);
		tss << csErrRef << std::endl;
	}
	catch(const _com_error& e)
	{
		tss<< _T("Exception in ") << (LPCTSTR)_bstr_t(__FUNCTION__) << _T(" ")
			<< std::hex << std::showbase<< e.Error()
			<< _T(", description ") << (e.ErrorMessage()? e.ErrorMessage(): (LPCTSTR)e.Description())<< std::endl;

	}
	Log(tss.str().c_str());
	wxConfigBase::Get()->Write(REGISTRY_PAGE_MACHINE, m_machine_input->GetValue());
	DisplayDuration(__FUNCTION__, m_start_time);
}

void Loki::ImplOf<RegistryPage>::OnWriteRegistry( wxCommandEvent& event )
{
	m_verbose_output->AppendText(wxString(__FUNCTION__));

}

void Loki::ImplOf<RegistryPage>::OnClearLog( wxCommandEvent& event )
{
	m_verbose_output->AppendText(wxString(__FUNCTION__));
	m_verbose_output->Clear();
}

RegistryPage::RegistryPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	d->CreateUI(this);
}


RegistryPage::~RegistryPage(void)
{
}

void RegistryPage::OnButton( wxCommandEvent& event )
{

}
