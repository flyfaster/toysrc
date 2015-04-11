#pragma once
class PythonPage :
	public wxPanel, private Loki::PimplOf<PythonPage>::Owner
{
public:
	PythonPage(wxWindow* parent);
	~PythonPage(void);
	void OnButton(wxCommandEvent& ev);
	static void WriteString(const char* str);
    void write( tchar_string const& str);
	DECLARE_EVENT_TABLE()

};
