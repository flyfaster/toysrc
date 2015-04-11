// -----------------------------------------------------------------------------
// File:             mydlg.h
// Description:
// Original author:  Onega Zhang
// Details:
//
// Notes:
//
// (C) Copyright 2006 - 2007, www.fruitfruit.com, All rights reserved.
// -----------------------------------------------------------------------------
#pragma once
typedef std::list<wxThread*> ThreadCollection;
typedef std::map<wxString, int> StringMap;
typedef std::list<tchar_string> tstringlist;
typedef boost::tokenizer<boost::char_separator<TCHAR>,
	tchar_string::const_iterator,
	tchar_string > tokenizer;

class ConfigPage;
class CopyPage;
class VerbosePage;
class FindPage;
class CrcPage;
class PathPage;
class CleanPage;
class CapturePage;
class PythonPage;
class RenamePage;
class Base64Page;

class DropFileEdit : public wxFileDropTarget
{
public:
	DropFileEdit(wxTextCtrl* edit):m_pedit(edit){}
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
	wxTextCtrl* m_pedit;
};

class PathUtilDlg: public wxFrame
{
public:
    PathUtilDlg();
    void OnButton(wxCommandEvent& event);
    void RemoveThread(wxThread* p);
    void OnCloseWindow(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& eventobj);
    void OnExit(wxCommandEvent& eventobj);
    void SaveConfiguration();
    void LoadConfiguration();
    void AddThread(wxThread* p);
    void Dump(std::string msg);
    void Dump(std::wstring msg);
    static void ChooseDirectory(wxWindow* parent, const wxString title, wxTextCtrl* receiver);
    static void DeleteSelections(wxListBox* listbox);
    static void AddExtensions(wxListBox* listbox, tchar_string body_prompt, tchar_string catpion);
    static wxXmlNode* GetNode(wxListBox* listbox, tchar_string group_name, tchar_string element_name);
    static void LoadNode(wxListBox* listbox, wxXmlNode* node, tchar_string group_name, tchar_string element_name);
	PathPage* m_path_page;
	CleanPage* m_clean_page;
	VerbosePage* m_verbose_page;
	ConfigPage* m_config_page;
	CopyPage* m_copy_page;
	FindPage* m_find_page;
	CrcPage* m_crc_page;
	CapturePage* m_capture_page;
	PythonPage* m_python_page;
	RenamePage* m_rename_page;
	Base64Page* m_base64_page;
private:
    wxButton* m_btnOK;
    wxButton* m_btnCancel;
    wxButton* m_btnExit;
    wxNotebook *m_book;
    ThreadCollection m_threads;
    wxCriticalSection m_cs;
    DWORD m_host_thread_id;
    DECLARE_EVENT_TABLE()
	void OnHotkey(wxKeyEvent& evt);
};



typedef Loki::SingletonHolder<PathUtilDlg, Loki::CreateUsingNew, Loki::NoDestroy/*Loki::PhoenixSingleton*/> SingleThreadsCol;
