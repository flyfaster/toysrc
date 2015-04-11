#pragma once 
class ConfigPage : public wxPanel//, public IPage
{
public:
    ConfigPage(wxWindow* parent);
    void Load();
    void Save();
    void OnButton(wxCommandEvent& event);
    wxString GetName()
    {
		wxString type_name =  wxConvLocal.cMB2WX(typeid(*this).name());
		int pos = type_name.find(_T(' '));
        if (pos!=wxString::npos)
		{
			return type_name.substr(pos+1);
		}
		return type_name;
    }
    wxXmlNode* GetNode();   
    void Init(wxXmlNode* p);
    wxString GetFileName();

private:
    wxButton *m_btnConfigFile;
    wxTextCtrl *m_txtConfigFile;
    wxButton *m_btnSave;
    wxButton *m_btnLoad;
    DECLARE_EVENT_TABLE()
};
