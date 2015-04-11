#pragma once
// http://msdn.microsoft.com/en-us/library/aa393664(VS.85).aspx
// StdRegProv is preinstalled in the WMI namespaces root\default and root\cimv2.

class RegistryPage :
	public wxPanel, private Loki::PimplOf<RegistryPage>::Owner
{
public:
	RegistryPage(wxWindow* parent);
	~RegistryPage(void);
	void OnButton(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

