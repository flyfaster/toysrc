// OzSvcClientDlg.h : header file
//

#pragma once

#include "afxwin.h"


// COzSvcClientDlg dialog
class OzIPCDlg : public CDialog
{
// Construction
public:
	OzIPCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OZSVCCLIENT_DIALOG };
	static void Dump(const std::string s);
	static void Dump(const std::wstring s);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedStartServer();
	afx_msg void OnBnClickedStartClient();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedMemoryTest();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnCbnSelendokShareType();
	afx_msg void OnBnClickedUpdateClient();
	afx_msg void OnEnChange(UINT nID );
	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	CComboBox m_share_type;
	int m_old_selection;

    void InitBenchmarkTest();
    void BenchmarkWindowsSharedMemoryServer();
    void BenchmarkRemoteMemoryServer();
    void BenchmarkPosixSharedMemoryServer();
    void StartCopyDataSender();
    void StartSockListener();
    void StartSockSender();
    void StartBoostAsioListener();
    void StartBoostAsioSender();
    void StartNamedPipeServer();
    void StartNamedPipeClient();
    void StartMailslotServer();
    void StartMailslotClient();
    void IPCTestClient();
	void OnBnClickedOk();
void OnBnClickedTest();
void OzIPCDlg::OnBnClickedTest2();
void OzIPCDlg::OnBnClickedGetFileInfo();
void OzIPCDlg::OnBnClickedStartService();
void OzIPCDlg::OnBnClickedStopService();
void OzIPCDlg::OnBnClickedUpload();

};
