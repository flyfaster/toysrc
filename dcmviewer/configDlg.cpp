// onfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dcmviewer.h"
#include "configDlg.h"
#include ".\configdlg.h"
#include "dcmviewer_const.h"
#include <atlbase.h>
// ConfigDlg dialog

IMPLEMENT_DYNAMIC(ConfigDlg, CDialog)
ConfigDlg::ConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigDlg::IDD, pParent)
{
}

ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIG_ITEMS, m_lv);
}


BEGIN_MESSAGE_MAP(ConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// ConfigDlg message handlers

BOOL ConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_lv.GetHeaderCtrl()->GetItemCount()==0)
	{
		m_lv.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		m_lv.ModifyStyle(0, LVS_REPORT | LVS_EDITLABELS);
		m_lv.InsertColumn(0,"Name");
		m_lv.InsertColumn(1,"Value");
		m_lv.SetColumnWidth(0,200);
		m_lv.SetColumnWidth(1,200);
		m_lv.AddEditableCol(1);
		//::SetWindowLong( m_lv.m_hWnd, GWL_STYLE, ::GetWindowLong(m_lv.m_hWnd, GWL_STYLE) | LVS_REPORT | LVS_EDITLABELS);

		//m_lv.SetExtendedStyle(LVS_EX_GRIDLINES|m_lv.GetExtendedStyle());
		CRegKey reg;
		if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE, DCMVIEWER_REG_PATH,KEY_READ))
		{
			int index = 0;
			TCHAR valname[MAX_PATH];
			valname[0] = 0;
			DWORD valnamelen = 0;
			TCHAR data[MAX_PATH];
			DWORD buflen = sizeof(data);
			while(true)
			{
				buflen = sizeof(data)/sizeof(data[0]);
				valnamelen = sizeof(valname)/sizeof(valname[0]);
				DWORD dwret = RegEnumValue(reg.m_hKey,index,valname,&valnamelen,NULL,NULL,(LPBYTE)data,&buflen);
				if(dwret!=ERROR_SUCCESS)
					break;
				else
				{
					if(valname>0)
					{
						int item = m_lv.GetItemCount();
						m_lv.InsertItem(item,valname);
						if(buflen>0)
							m_lv.SetItemText(item,1,data);
					}
				}
				index++;
			}
			
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ConfigDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CRegKey reg;
	if(ERROR_SUCCESS == reg.Open(HKEY_LOCAL_MACHINE,DCMVIEWER_REG_PATH))
	{
		TCHAR buf[MAX_PATH];
		TCHAR name[MAX_PATH];
		DWORD len = 0;
		for(int i=0;i<m_lv.GetItemCount();i++)
		{
			len = sizeof(name)/sizeof(name[0]);
			m_lv.GetItemText(i,0,name,len);
			len = sizeof(name)/sizeof(name[0]);
			m_lv.GetItemText(i,1,buf,len);
			reg.SetStringValue(name,buf);
		}
		reg.Close();
		AfxMessageBox(_T("please restart the application in order to make settings in effect"));
	}
	else
	{
		AfxMessageBox(_T("Cant open registry"));
	}
	OnOK();
}
