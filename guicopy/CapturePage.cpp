#include "guicopy_include.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#include "mydlg.h"

#include "CapturePage.h"
#include "mainapp.h"
#include "Card.h"
#include "CardView.h"

BEGIN_EVENT_TABLE(CapturePage, wxPanel)
EVT_BUTTON(wxID_ANY, CapturePage::OnButton)
EVT_HOTKEY(CapturePage::hotkey_id, CapturePage::OnHotkey)
END_EVENT_TABLE()

CapturePage::CapturePage(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
	InitCardSuit();
	m_process_id = new wxTextCtrl(this, wxID_ANY);
	m_process_name = new wxTextCtrl(this, wxID_ANY);
	m_width = new wxTextCtrl(this, wxID_ANY);
	m_height = new wxTextCtrl(this, wxID_ANY);
	m_parent_caption = new wxTextCtrl(this, wxID_ANY);
	m_self_caption = new wxTextCtrl(this, wxID_ANY);
	m_hwnd = new wxTextCtrl(this, wxID_ANY);
	m_parent_class_name = new wxTextCtrl(this, wxID_ANY);
	m_self_class_name = new wxTextCtrl(this, wxID_ANY);
	m_capture_height = new wxTextCtrl(this, wxID_ANY);
	m_capture_width = new wxTextCtrl(this, wxID_ANY);
	m_save_dir = new wxTextCtrl(this, wxID_ANY);
	m_browse_save_dir = new wxButton(this, wxID_ANY, wxT("Browse dir..."));
	m_card_view = new CardView(this);
	m_width->SetValue(wxT("602"));
	m_height->SetValue(wxT("570"));
	m_process_name->SetValue(wxT("Frame.exe"));
	m_capture_width->SetValue(wxT("530"));
	m_capture_height->SetValue(wxT("530"));

	wxGridBagSizer* sizer = new wxGridBagSizer();
	int row = 0;
	int column = 0;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Process ID")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_process_id, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Process Name")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_process_name, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Width")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_width, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Height")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_height, wxGBPosition(row,column));
	row++;
	column=0;
	sizer->Add(new wxStaticText(this,wxID_ANY, _T("Parent caption")), wxGBPosition(row,column));
	column++;
	sizer->Add(m_parent_caption, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Self caption")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_self_caption, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Parent class name")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_parent_class_name, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Self class name")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_self_class_name, wxGBPosition(row,column));
	row++;
	column=0;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("HWND")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_hwnd, wxGBPosition(row,column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Capture width")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_capture_width, wxGBPosition(row, column));
	column++;
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Capture height")), wxGBPosition(row, column));
	column++;
	sizer->Add(m_capture_height, wxGBPosition(row, column));
	row++;
	column=0;
	sizer->Add(m_browse_save_dir, wxGBPosition(row, column));
	column++;
	sizer->Add(m_save_dir, wxGBPosition(row, column), wxGBSpan(1, 7), wxEXPAND | wxRIGHT);
	row++;
	column=0;
	sizer->Add(m_card_view, wxGBPosition(row, column), wxGBSpan(3, 8), wxEXPAND | wxALL);
	sizer->AddGrowableCol(7);
	sizer->AddGrowableRow(row);
	wxGetApp().SetSizer(this, sizer);
}

CapturePage::~CapturePage(void)
{

}

void CapturePage::OnButton( wxCommandEvent& evt )
{
	wxButton* active_button = dynamic_cast<wxButton*>(evt.GetEventObject());
	if (active_button)
	{
		wxLogDebug(active_button->GetLabelText().c_str());
		if (active_button->GetLabelText()==wxT("OK"))
		{
			DWORD start = GetTickCount();
			OnOK();
			start = GetTickCount()-start;
			std::basic_stringstream<TCHAR> ts;
			ts << _T("Capture use ") << start << _T(" milliseconds");
			wxLogVerbose(ts.str().c_str());
		}
	}
	else
	if (evt.GetEventObject()==m_browse_save_dir)
	{
		PathUtilDlg::ChooseDirectory(this, wxT("Set file save directory"), m_save_dir);
	}
	else
		evt.Skip(true);
}

int CapturePage::CaptureWnd(HWND wnd, tchar_string filename, int width, int height)
{
	// alternative: http://www.fengyuan.com/article/wmprint.html
	if (!IsWindow(wnd))
		return ERROR_INVALID_PARAMETER;
	BringWindowToTop(wnd);
	Sleep(10);
	RECT rect;
	HDC dc = GetDC(wnd);
	::GetClientRect(wnd, &rect);
	HDC dc1 = CreateCompatibleDC(dc);
	if (width>0)
		rect.right = rect.left+width;
	if (height>0)
		rect.bottom = rect.top+height;
	BITMAPINFO bi;
	memset( &bi, 0, sizeof(BITMAPINFO) );
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = rect.right-rect.left;
	bi.bmiHeader.biHeight = rect.bottom-rect.top;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	void *bits = NULL;
	HBITMAP bm = CreateDIBSection(dc1, &bi, DIB_RGB_COLORS,
		(void **)(&bits), NULL, 0 );

	SelectObject(dc1, bm);
	BitBlt(dc1, rect.left, rect.top, bi.bmiHeader.biWidth, 
		bi.bmiHeader.biHeight, dc, 
		rect.left, rect.top, SRCCOPY);
	//SendMessage(wnd, WM_PRINTCLIENT,reinterpret_cast<WPARAM>(dc1),PRF_CLIENT);
	wxImage image;
	image.Create(rect.right, rect.bottom);
	for (int x = 0; x < rect.right; x++)
	{
		for (int y = 0; y < rect.bottom; y++)
		{
			COLORREF c = GetPixel(dc1, x, y);
			image.SetRGB(x, y, GetRValue(c), GetGValue(c), GetBValue(c));
		}
	}
	image.SaveFile(filename, wxBITMAP_TYPE_BMP); 
	return 0;
}

void CapturePage::OnOK()
{
	// parent caption
	// window size
	// self caption
	// process ID
	// process name
	// HWND
	return test();
	m_target_wnd = NULL;
	m_pid = 0;
	if (m_capture_width->GetValue().size())
		capture_width = _ttoi(m_capture_width->GetValue().c_str());
	if (m_capture_height->GetValue().size())
		capture_height = _ttoi(m_capture_height->GetValue().c_str());

	wxString temp_str = m_hwnd->GetValue();
	if (temp_str.size())
	{
		DWORD temp;
		_stscanf(temp_str.c_str(), wxT("%x"), &temp);
		CaptureWnd((HWND)temp, wxT("test.bmp"), capture_width, capture_height);
		return;
	}
	temp_str = m_process_id->GetValue();
	if (temp_str.size())
	{
		m_pid = _ttoi(temp_str.c_str());
	}
	temp_str = m_process_name->GetValue();
	if (m_pid==0 && temp_str.size())
	{
		// get pid by name
		const DWORD buf_size = 4096;
		DWORD size_needed = 0;
		boost::scoped_array<DWORD> pid_buf(new DWORD[buf_size]);
		EnumProcesses(pid_buf.get(), buf_size, &size_needed);
		DWORD cProcesses = size_needed / sizeof(DWORD);		
		for (DWORD i=0; i<cProcesses; i++)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, pid_buf[i] );
			// Get the process name.
			if (NULL != hProcess )
			{
				HMODULE hMod;
				DWORD cbNeeded;
				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
					&cbNeeded) )
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, 
						sizeof(szProcessName)/sizeof(TCHAR) );
				}
			}
			CloseHandle( hProcess );
			if (_tcsicmp(szProcessName, temp_str.c_str())==0)
			{
				m_pid = pid_buf[i];
				break;
			}
		}
	}
	if (m_pid>0)
	{
		std::basic_stringstream<TCHAR> ts;
		ts <<std::dec<< m_pid;
		m_process_id->SetValue(ts.str());
		m_process_name->SetValue(wxT(""));
	}
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
	if (m_target_wnd==NULL)
		wxLogWarning(_T("Unable to find window"));
}

void CapturePage::OnHotkey( wxKeyEvent& evt )
{
    wxLogDebug(wxString::FromAscii(__FUNCTION__));
	OnOK();
}

BOOL CALLBACK CapturePage::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	// if process id is specified
	CapturePage* page = (CapturePage*)lParam;
	if (page->m_pid)
	{
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		if (pid != page->m_pid)
			return TRUE;
	}
	BOOL ret = EnumChildWindows(hwnd, EnumChildProc, lParam);
	return page->m_target_wnd == NULL;
}

BOOL CALLBACK CapturePage::EnumChildProc( HWND hwnd, LPARAM lParam )
{
	CapturePage* page = (CapturePage*)lParam;
	if (page->m_width->GetValue().size() && page->m_height->GetValue().size())
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		if ( 
			   ((rc.right-rc.left)==_ttoi(page->m_width->GetValue().c_str()))
			&& ((rc.bottom-rc.top)==_ttoi(page->m_height->GetValue().c_str()))
			)
		{
			page->m_target_wnd = hwnd;
			tchar_string filename = page->GetSavePath();
			CaptureWnd((HWND)page->m_target_wnd, filename, page->capture_width, page->capture_height);
			return FALSE;
		}
	}
	return TRUE;
}

tchar_string CapturePage::GetSavePath()
{
	using namespace boost::date_time;
	using namespace boost::posix_time;
	using namespace boost::local_time;
	ptime current_time = boost::posix_time::microsec_clock::local_time();
	typedef boost::date_time::time_facet<local_date_time, TCHAR> tlocal_time_facet;
	tlocal_time_facet* output_facet=new tlocal_time_facet();
	std::basic_stringstream<TCHAR> ts;
	boost::posix_time::time_duration t = current_time.time_of_day();
	tchar_string save_dir = m_save_dir->GetValue();
	if (save_dir.size())
	{
		boost::filesystem::create_directory(save_dir.c_str());
		ts << save_dir;
		if (save_dir.at(save_dir.size()-1) != _T('\\'))
		{
			ts << _T('\\');
			m_save_dir->SetValue(ts.str());
		}
	}
	ts << m_width->GetValue() << wxT("X") << m_height->GetValue()
		<< wxT("_")
		<< std::setw(2) << std::setfill(wxT('0')) << boost::date_time::absolute_value(t.hours())
		<< std::setw(2) << std::setfill(wxT('0')) << boost::date_time::absolute_value(t.minutes())
		<< std::setw(2) << std::setfill(wxT('0')) << boost::date_time::absolute_value(t.seconds())
		<< wxT(".bmp");
	tchar_string filename = ts.str ();
	return filename;
}



int CapturePage::FindCard( const wxImage& src, const wxRect& psrc_rect, const wxImage& cards )
{
	wxRect card_size(0, 0, 60, 90);
	wxRect compare_size(0, 0, Card::VisibleWidth(), Card::VisibleHeight());
	wxRect src_rect = psrc_rect;
	src_rect.SetWidth(compare_size.GetWidth());
	src_rect.SetHeight(compare_size.GetHeight());
	wxRect card_position;
	card_position.SetWidth(compare_size.GetWidth());
	card_position.SetHeight(compare_size.GetHeight());
	for (int i=0; i< 55; i++)
	{
		int row = i/13;
		int col = i%13;
		card_position.SetLeft(col*card_size.GetWidth());
		card_position.SetTop(row*card_size.GetHeight());
		if (Card::WhiteMatch(src, cards, src_rect, card_position))
			return i;
	}
	return -1;
}

void CapturePage::test()
{
	std::basic_stringstream<TCHAR> ts;
	wxImage src_img;
	LPCTSTR src_file_name = wxT("E:\\svn\\guicopy\\602X570_221117.bmp");
	bool load_ret = src_img.LoadFile(src_file_name, wxBITMAP_TYPE_BMP);
	if (!load_ret)
	{
		ts << _T("Failed to load ") << src_file_name;
		wxLogError(ts.str().c_str());
		return;
	}
	LPCTSTR cards_file_name = wxT("E:\\svn\\guicopy\\dalu\\bmp184.bmp");
	load_ret = m_cards.LoadFile(cards_file_name, wxBITMAP_TYPE_BMP);
	if (!load_ret)
	{
		ts << _T("Failed to load ") << cards_file_name;
		wxLogError(ts.str().c_str());
		return;
	}
	m_card_view->Freeze();
	wxRect src_rect(220,40, 60, 90);
	//for (int i=0; i< 5; i++)
	//{
	//	ts.str(wxT(""));
	//	src_rect.SetLeft(220+12*i);
	//	int card = FindCard(src_img, src_rect, m_cards);
	//	ts << i << wxT(" card is ") << std::dec << Card::GetName(card) << wxT(", position:")
	//		<< src_rect.GetLeft() << wxT(", ") << src_rect.GetTop()
	//		<< wxT(", ") << src_rect.GetRight() << wxT(", ") << src_rect.GetBottom();
	//	wxGetApp().Dump(ts.str());
	//}
	// 220,250
	//src_rect.SetLeft(220);
	//src_rect.SetTop(250);
	//for (int i=0; i< 5; i++)
	//{
	//	ts.str(wxT(""));
	//	src_rect.SetLeft(220+12*i);
	//	int card = FindCard(src_img, src_rect, m_cards);
	//	ts << i << wxT(" card is ") << std::dec << Card::GetName(card) << wxT(", position:")
	//		<< src_rect.GetLeft() << wxT(", ") << src_rect.GetTop()
	//		<< wxT(", ") << src_rect.GetRight() << wxT(", ") << src_rect.GetBottom();
	//	wxGetApp().Dump(ts.str());
	//}
	// 80,200
	// 352,90
	// (220,40)	(80,90)	(352,90)	(80,200)	(352,200)	(220,250)
	wxRect card_positions[7]; //{{220,40}, {80,90}, {352,90}, {80,200}, {352,200}, {220,250}};
	card_positions[0] .SetLeftTop(wxPoint(220,40)) ;
	card_positions[1] .SetLeftTop(wxPoint(80,90)) ;
	card_positions[2] .SetLeftTop(wxPoint(352,90)) ;
	card_positions[3] .SetLeftTop(wxPoint(80,200)) ;
	card_positions[4] .SetLeftTop(wxPoint(352,200)) ;
	card_positions[5] .SetLeftTop(wxPoint(220,250));
	for (int i=0; i<sizeof(card_positions)/sizeof(*card_positions); i++)
	{
		card_positions[i].SetWidth(5*Card::VisibleWidth());
		card_positions[i].SetHeight(Card::VisibleHeight());
	}
	card_positions[6].SetLeft(88);
	card_positions[6].SetTop(400);
	card_positions[6].SetWidth(27*Card::VisibleWidth());
	for (int card_pos=0; card_pos<sizeof(card_positions)/sizeof(*card_positions); card_pos++)
	{
		wxRect& current_range = card_positions[card_pos];
		//src_rect = first_card_pos;
		for (int i=0; i< current_range.GetWidth()/Card::VisibleWidth(); i++)
		{
			ts.str(wxT(""));
			src_rect.SetLeft(current_range.x+Card::VisibleWidth()*i);
			src_rect.SetTop(current_range.y);
			int card = FindCard(src_img, src_rect, m_cards);
			SingleCardSuit::Instance().Remove(card);
			ts << i << wxT(" card is ") << std::dec << Card::GetName(card) << wxT(", position:")
				<< src_rect.GetLeft() << wxT(", ") << src_rect.GetTop()
				<< wxT(", ") << src_rect.GetRight() << wxT(", ") << src_rect.GetBottom();
			wxLogVerbose(ts.str().c_str());
			if (card<0 || card>=54)
				break;
		}

	}
	m_card_view->Thaw();
	m_card_view->Refresh();
}

void CapturePage::InitCardSuit()
{
	for (int i=0; i<54; i++)
	{
		SingleCardSuit::Instance()[Card(i)] = 3;
	}
}

wxBitmap CapturePage::GetScreenShot()
{
	wxSize screenSize = wxGetDisplaySize();
	wxBitmap bitmap(screenSize.x, screenSize.y);
	wxScreenDC dc;
	wxMemoryDC memDC;
	memDC.SelectObject(bitmap);
	memDC.Blit(0, 0, screenSize.x, screenSize.y, & dc, 0, 0);
	memDC.SelectObject(wxNullBitmap);
	return bitmap;
}