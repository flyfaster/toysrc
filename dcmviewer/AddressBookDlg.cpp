// AddressBookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "AddressBookDlg.h"
#include "dcmviewer_const.h"
#include <boost/tokenizer.hpp>

// AddressBookDlg dialog

IMPLEMENT_DYNAMIC(AddressBookDlg, CDialog)
AddressBookDlg::AddressBookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddressBookDlg::IDD, pParent)
{
}

AddressBookDlg::~AddressBookDlg()
{
}

void AddressBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDRESS_NAME, m_cmbname);
	DDX_Control(pDX, IDC_IP, m_ip);
}


BEGIN_MESSAGE_MAP(AddressBookDlg, CDialog)
	ON_BN_CLICKED(IDB_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDB_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDB_DELETE, OnBnClickedDelete)
	ON_CBN_SELENDOK(IDC_ADDRESS_NAME, OnCbnSelendokAddressName)
END_MESSAGE_MAP()


// AddressBookDlg message handlers

void AddressBookDlg::OnBnClickedAdd()
{
	SetDlgItemText(IDC_ADDRESS_NAME,_T(""));
	SetDlgItemText(IDE_LOCAL_AE,_T(""));
	SetDlgItemText(IDE_REMOTE_AE,_T(""));
	SetDlgItemText(IDE_PORT,_T("104"));
	m_ip.SetAddress(0,0,0,0);
}

void AddressBookDlg::OnBnClickedSave()
{
	TCHAR buf[1024];
	buf[0] = 0;
	GetDlgItemText(IDC_ADDRESS_NAME,buf,sizeof(buf)/sizeof(buf[0]));
	std::basic_string<TCHAR> addressname = buf;
	if(_tcslen(buf)<=0 || _tcslen(buf)>=MAX_PATH)
	{
		return;
	}
	GetDlgItemText(IDE_REMOTE_AE,buf,sizeof(buf)/sizeof(buf[0]));
	std::basic_string<TCHAR> remoteae = buf;
	if(_tcslen(buf)<=0 || _tcslen(buf)> MAX_AE_LEN)
	{
		AfxMessageBox(_T("invalid remote AE"));
		return;
	}
	GetDlgItemText(IDE_LOCAL_AE,buf,sizeof(buf)/sizeof(buf[0]));
	std::basic_string<TCHAR> local_ae = buf;

	GetDlgItemText(IDE_PORT,buf,sizeof(buf)/sizeof(buf[0]));
	std::basic_string<TCHAR> portstr = buf;
	if( _ttoi(buf)<=(std::numeric_limits<unsigned short>::min)() || _ttoi(buf)>=(std::numeric_limits<unsigned short>::max)())
	{
		AfxMessageBox(_T("invalid port number"));
		return;
	}

	std::basic_ostringstream<TCHAR> oss;
	oss<<ADDRESS_BOOK_PATH<<"\\"<<addressname;
	CRegKey reg;
	reg.Create(HKEY_LOCAL_MACHINE,oss.str().c_str());
	reg.SetStringValue(_T("remote_ae"),remoteae.c_str());
	reg.SetStringValue(_T("local_ae"),local_ae.c_str());
	reg.SetStringValue(_T("port"),portstr.c_str());
	oss.str("");
	BYTE ip[4];
	m_ip.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	oss<<(int(ip[0])&0xff)<<"."<<(int(ip[1])&0xff)<<"."<<(int(ip[2])&0xff)<<"."<<(int(ip[3])&0xff);
	reg.SetStringValue(_T("ip"),oss.str().c_str());
	reg.Close();
	m_cmbname.AddString(addressname.c_str());
}

void AddressBookDlg::OnBnClickedDelete()
{
	// TODO: Add your control notification handler code here
}

BOOL AddressBookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	FillCmb(m_cmbname);
	//CRegKey reg;
	//if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,ADDRESS_BOOK_PATH,KEY_ENUMERATE_SUB_KEYS | KEY_READ | KEY_WRITE))
	//{
	//	DWORD index = 0;
	//	LONG lret = ERROR_SUCCESS;
	//	TCHAR namebuf[MAX_PATH];
	//	DWORD len = 0;
	//	do {
	//		namebuf[0] = 0;
	//		len = sizeof(namebuf)/sizeof(namebuf[0]);
	//		lret = reg.EnumKey(index,namebuf,&len,NULL);
	//		if(ERROR_SUCCESS == lret && len>0)
	//		{
	//			m_cmbname.AddString(namebuf);
	//			index++;
	//		}
	//	} while(lret == ERROR_SUCCESS);
	//}
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AddressBookDlg::FillCmb(CComboBox &cmb)
{
	while(cmb.GetCount()>0)
		cmb.DeleteString(0);
	CRegKey reg;
	if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,ADDRESS_BOOK_PATH,KEY_ENUMERATE_SUB_KEYS | KEY_READ | KEY_WRITE))
	{
		DWORD index = 0;
		LONG lret = ERROR_SUCCESS;
		TCHAR namebuf[MAX_PATH];
		DWORD len = 0;
		do {
			namebuf[0] = 0;
			len = sizeof(namebuf)/sizeof(namebuf[0]);
			lret = reg.EnumKey(index,namebuf,&len,NULL);
			if(ERROR_SUCCESS == lret && len>0)
			{
				cmb.AddString(namebuf);
				index++;
			}
		} while(lret == ERROR_SUCCESS);
	}

}
void AddressBookDlg::OnCbnSelendokAddressName()
{
	TCHAR buf[1024];
	buf[0] = 0;
	m_cmbname.GetLBText(m_cmbname.GetCurSel(),buf);
	std::basic_string<TCHAR> addressname = buf;
	if(_tcslen(buf)<=0 || _tcslen(buf)>=MAX_PATH)
	{
		SetDlgItemText(IDC_ADDRESS_NAME,_T(""));
		SetDlgItemText(IDE_LOCAL_AE,_T(""));
		SetDlgItemText(IDE_REMOTE_AE,_T(""));
		SetDlgItemText(IDE_PORT,_T("104"));
		m_ip.SetAddress(0,0,0,0);
		return;
	}
	std::basic_ostringstream<TCHAR> oss;
	oss<<ADDRESS_BOOK_PATH<<"\\"<<addressname;
	CRegKey reg;
	if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,oss.str().c_str()))
	{
		TCHAR valbuf[MAX_PATH];
		LONG ret = ERROR_SUCCESS;
		ULONG len = 0;
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("remote_ae"),valbuf,&len);
		SetDlgItemText(IDE_REMOTE_AE,valbuf);
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("local_ae"),valbuf,&len);
		SetDlgItemText(IDE_LOCAL_AE,valbuf);
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("port"),valbuf,&len);
		SetDlgItemText(IDE_PORT,valbuf);
		valbuf[0] = 0;
		len = sizeof(valbuf);
		ret = reg.QueryStringValue(_T("ip"),valbuf,&len);
		typedef std::basic_string<TCHAR> tchar_string;
		typedef boost::tokenizer<boost::char_separator<TCHAR>,tchar_string::const_iterator,tchar_string > tokenizer;
		boost::char_separator<TCHAR> sep_component(_T("."),NULL, boost::drop_empty_tokens);
		tchar_string str = valbuf;
		tokenizer component_tokens( str, sep_component);
		tokenizer::iterator component_iter = component_tokens.begin();
		int index = 0;
		BYTE ip[4];
		memset(ip,0,sizeof(ip));
		while(component_iter!=component_tokens.end() && index<sizeof(ip)/sizeof(ip[0]))
		{
			tchar_string ws_component = *component_iter;
			if(ws_component.length()>0)
			{
				ip[index] = _ttoi(ws_component.c_str());
			}
			component_iter++;
			index++;
		}
		m_ip.SetAddress(ip[0],ip[1],ip[2],ip[3]);

	}

}
