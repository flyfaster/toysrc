#include "guicopy_include.h"
#include <cryptlib.h>
#include <iterhash.h>
#include <crc.h>
#include <md5.h>
#include <sha.h>
#include <factory.h>
#include "CrcPage.h"
#include "mydlg.h"

BEGIN_EVENT_TABLE(CrcPage, wxPanel)
EVT_BUTTON(wxID_ANY, CrcPage::OnButton)
END_EVENT_TABLE()

CrcPage::CrcPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	m_file_name = new wxTextCtrl(this, wxID_ANY );
	m_file_name->SetDropTarget(new DropFileEdit(m_file_name));
	m_choose_file = new wxButton(this, wxID_ANY, _T("File..."));
	m_copy_checksum = new wxButton(this, wxID_ANY, _T("Checksum"));
	m_checksum = new wxTextCtrl(this, wxID_ANY);
	m_checksum_type = new wxComboBox(this, wxID_ANY);
	m_checksum_type->AppendString(_T("CRC32"));
	m_checksum_type->AppendString(_T("MD5"));
	m_checksum_type->AppendString(wxT("SHA"));
	m_checksum_type->AppendString(wxT("SHA1"));
	m_checksum_type->AppendString(wxT("SHA256"));
	m_checksum_type->AppendString(wxT("SHA224"));
	m_checksum_type->AppendString(wxT("SHA512"));
	m_checksum_type->Select(0);
	int row = 0;
	int col = 0;
	wxGridBagSizer* sizer = new wxGridBagSizer();
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Checksum type")), wxGBPosition(row, col));
	col++;
	sizer->Add(m_checksum_type, wxGBPosition(row, col));
	row++;
	col = 0;
	sizer->Add(m_choose_file, wxGBPosition(row, col));
	col++;
	sizer->Add(m_file_name, wxGBPosition(row, col), wxDefaultSpan, wxEXPAND | wxRIGHT);
	row++;
	col = 0;
	sizer->Add(m_copy_checksum, wxGBPosition(row, col));
	col++;
	sizer->Add(m_checksum, wxGBPosition(row, col), wxDefaultSpan, wxEXPAND | wxRIGHT);
	sizer->AddGrowableCol(1);
	this->SetSizer(sizer);
	sizer->SetSizeHints(this);
	sizer->Fit(this);
}

CrcPage::~CrcPage(void)
{
}

wxString CrcPage::GetName()
{
	return wxT("CrcPage");
}

template <class T>
tchar_string get_checksum(tchar_string filename, T* checksum_algorithm)
{
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	std::ifstream src_file(filename.c_str(), std::ios::binary);
	if (src_file.good()==false)
		return wxT("");
	do 
	{
		src_file.read(buf, sizeof(buf));
		if (src_file.gcount()>0)
		{
			checksum_algorithm->Update( (const byte*)buf, src_file.gcount());
		}
	} while (src_file.good());
	checksum_algorithm->Final( (byte*)buf);
	std::basic_stringstream<TCHAR> ss;
	for (size_t i=0; i<checksum_algorithm->DigestSize(); i++)
	{
		ss << std::hex << std::uppercase << std::setfill(wxT('0')) 
			<< std::setw(2) << std::internal << ((int)buf[i]&0xff);
	}
	return ss.str();
}

void CrcPage::OnOK()
{
	wxString filename = m_file_name->GetValue();
	if (filename.size()==0)
		return;
	std::ifstream src_file(filename.wx_str(), std::ios::binary);
	if (src_file.good()==false)
		return;
	wxString checksum_type = m_checksum_type->GetValue();
	using namespace CryptoPP;
    //ObjectFactoryRegistry<HashTransformation>::Registry();
    //ObjectFactoryRegistry<HashTransformation>::Registry().CreateObject("CRC32");
	if (checksum_type==wxT("CRC32"))
	{
		CRC32 checksum;
        m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
  //      HashTransformation* ptransform = ObjectFactoryRegistry<HashTransformation>::Registry().CreateObject("CRC32");
		//m_checksum->SetValue(get_checksum(filename.wx_str(), ptransform).c_str());;
	}
	if (checksum_type==wxT("MD5"))
	{
		Weak1::MD5 checksum;
		m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
        //HashTransformation* ptransform = ObjectFactoryRegistry<HashTransformation>::Registry().CreateObject("MD5");
        //m_checksum->SetValue(get_checksum(filename.wx_str(), ptransform).c_str());;

	}
	if (checksum_type==wxT("SHA") || checksum_type==wxT("SHA1"))
	{
		SHA checksum;
		m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
	}
	if (checksum_type==wxT("SHA256"))
	{
		SHA256 checksum;
		m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
	}
	if (checksum_type==wxT("SHA224"))
	{
		SHA224 checksum;
		m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
	}
	if (checksum_type==wxT("SHA512"))
	{
		SHA512 checksum;
		m_checksum->SetValue(get_checksum(filename.wx_str(), &checksum).c_str());;
	}
}

void CrcPage::OnButton( wxCommandEvent& eventobj )
{
	if (eventobj.GetEventObject()==m_choose_file)
	{
		wxFileDialog* openFileDialog =
			new wxFileDialog( this, _T("Open file"), wxT(""), wxT(""), wxT("Any file(*.*)|*.*"),
			wxFD_OPEN, wxDefaultPosition);

		if ( openFileDialog->ShowModal() == wxID_OK )
		{
			m_file_name->SetValue(openFileDialog->GetPath());
			m_checksum->SetValue(_T(""));
		}
		
	}
	if (eventobj.GetEventObject()==m_copy_checksum)
	{
		wxTheClipboard->SetData(new wxTextDataObject(m_checksum->GetValue()));
	}
	wxButton* active_button = dynamic_cast<wxButton*>(eventobj.GetEventObject());
	if (active_button)
	{
		OutputDebugString(active_button->GetLabelText());
		if (active_button->GetLabelText()==wxT("OK"))
		{
			OnOK();
		}
	}

}