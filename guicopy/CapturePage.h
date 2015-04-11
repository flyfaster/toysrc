#pragma once

class CardView;

class CapturePage :
	public wxPanel
{
public:
	CapturePage(wxWindow* parent);
	~CapturePage(void);
	enum { hotkey_id = 112};
private:
	DECLARE_EVENT_TABLE();
	wxTextCtrl* m_process_id;
	wxTextCtrl* m_process_name;
	wxTextCtrl* m_width;
	wxTextCtrl* m_height;
	wxTextCtrl* m_parent_caption;
	wxTextCtrl* m_self_caption;
	wxTextCtrl* m_hwnd;
	wxTextCtrl* m_self_class_name;
	wxTextCtrl* m_parent_class_name;
	wxTextCtrl* m_capture_width;
	wxTextCtrl* m_capture_height;
	wxTextCtrl* m_save_dir;
	wxButton* m_browse_save_dir;
	CardView* m_card_view;
	HWND m_target_wnd;
	DWORD m_pid;
	void OnButton(wxCommandEvent& evt);
	void OnHotkey(wxKeyEvent& evt);
	static int CaptureWnd(HWND hwnd, tchar_string filename, int width, int height);
	void OnOK();
	static BOOL CALLBACK EnumWindowsProc(          HWND hwnd,
		LPARAM lParam
		);
	static BOOL CALLBACK EnumChildProc(          HWND hwnd,
		LPARAM lParam
		);
	static int FindCard(const wxImage& src, const wxRect& src_rect, const wxImage& cards);
	int capture_width;
	int capture_height;
	tchar_string GetSavePath();
	wxImage m_cards;
	void test();
	void InitCardSuit();
	wxBitmap GetScreenShot();
};
