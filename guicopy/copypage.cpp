#include "guicopy_include.h"
#include "mainapp.h"
#include "mydlg.h"
#include "CopyPage.h"
#include "CopyThread.h"


LPCTSTR CopyPage::COPY_PAGE_INCLUDE_EXTENSIONS_NODE = _T("IncludeExtensions");

BEGIN_EVENT_TABLE(CopyPage, wxPanel)
EVT_BUTTON(wxID_ANY, CopyPage::OnButton)
END_EVENT_TABLE()

CopyPage::CopyPage( wxWindow* parent ):wxPanel(parent, wxID_ANY)
{
    wxGridBagSizer* sizer = new wxGridBagSizer();
    m_copy_choose_destination = new wxButton(this, wxID_ANY, _T("Destination..."));
    m_copy_choose_source = new wxButton(this, wxID_ANY, _T("Source..."));
    m_copy_add_ext = new wxButton(this, wxID_ANY, _T("Add Extension..."));
    m_copy_remove_ext = new wxButton(this, wxID_ANY, _T("Remove Selection"));
    m_copy_ext_list = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0
        , NULL, wxLB_SORT|wxLB_EXTENDED);
    m_copy_source = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    m_copy_destination = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    wxString labels[] = { wxT("include"), wxT("exclude")};
    m_copy_ext_flag = new wxRadioBox(this, wxID_ANY, _T("Extensions to use"),
        wxDefaultPosition, wxDefaultSize, 2, labels, 0, wxRA_VERTICAL);
    m_copy_ext_flag->SetString(0, _T("Include these files"));
    m_copy_ext_flag->SetString(1, _T("Exclude these files"));
    m_add_timestamp = new wxCheckBox(this, wxID_ANY, wxT("Add timestamp"));
    m_operation_type = new wxComboBox(this, wxID_ANY);
    m_operation_type->AppendString(wxT("Copy"));
    m_operation_type->AppendString(wxT("Move"));
    m_operation_type->AppendString(wxT("Rename"));
    m_operation_type->SetValue(wxT("Copy"));
    int row = 0;
    int column = 0;
    sizer->Add(m_copy_choose_source, wxGBPosition(row, column));
    column++;
    sizer->Add(m_copy_source, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND | wxRIGHT);
    row++;
    column = 0;
    sizer->Add(m_copy_choose_destination, wxGBPosition(row, column));
    column++;
    sizer->Add(m_copy_destination, wxGBPosition(row, column), wxDefaultSpan, wxEXPAND|wxRIGHT);
    row++;
    column = 0;
    sizer->Add(m_copy_add_ext, wxGBPosition(row, column));
    column++;
    sizer->Add(m_copy_ext_list, wxGBPosition(row, column), wxGBSpan(5,1), wxEXPAND|wxVERTICAL);
    row++;
    column = 0;
    sizer->Add(m_copy_remove_ext, wxGBPosition(row, column));
    row++;
    column = 0;
    sizer->Add(m_copy_ext_flag, wxGBPosition(row, column));
    row++;
    column = 0;
    sizer->Add(m_add_timestamp, wxGBPosition(row, column));
    row++; column=0;
    sizer->Add(m_operation_type, wxGBPosition(row, column));
    sizer->AddGrowableCol(1);
    sizer->AddGrowableRow(row);
    sizer->AddGrowableRow(0);

    this->SetSizer(sizer);
    sizer->SetSizeHints(this);
    sizer->Fit(this);

    m_copy_destination->SetDropTarget(new DropFileEdit(m_copy_destination));
    m_copy_source->SetDropTarget(new DropFileEdit(m_copy_source));

}
CopyPage::~CopyPage(void)
{
}

void CopyPage::OnButton( wxCommandEvent& eventObj )
{
	wxButton* active_button = dynamic_cast<wxButton*>(eventObj.GetEventObject());
	if (active_button)
	{
		OutputDebugString(active_button->GetLabelText());
		if (active_button->GetLabelText()==wxT("OK"))
		{
			OnOK();
            return;
		}
	}

    if(eventObj.GetEventObject()==m_copy_choose_destination)
    {
		PathUtilDlg::ChooseDirectory( &SingleThreadsCol::Instance(), _T("Choose destination directory"), m_copy_destination);
    }
    else if(eventObj.GetEventObject()==m_copy_choose_source)
    {
        PathUtilDlg::ChooseDirectory(&SingleThreadsCol::Instance(), _T("Choose source directory"), m_copy_source);
    }
    else if(eventObj.GetEventObject()==m_copy_add_ext)
    {
        PathUtilDlg::AddExtensions(m_copy_ext_list,
            _T("Enter extension names (prefixed with dot) to list, extensions can be separated by , ; or whitespace"),
            _T("Please enter extension names"));
    }
    else if(eventObj.GetEventObject()==m_copy_remove_ext)
    {
        PathUtilDlg::DeleteSelections(m_copy_ext_list);
    }
}

void CopyPage::OnOK()
{
    bool included = (m_copy_ext_flag->GetSelection()==0);
    CopyThread* copier = new CopyThread( m_copy_source->GetValue(), m_copy_destination->GetValue(),
        m_copy_ext_list->GetStrings(), included);
    copier->AddTimestampToFileName(m_add_timestamp->IsChecked());
    if (m_operation_type->GetValue() == wxT("Copy"))
        copier->SetActionType(CopyThread::DO_COPY);
    else if (m_operation_type->GetValue() == wxT("Move"))
        copier->SetActionType(CopyThread::DO_MOVE);
    else
        copier->SetActionType(CopyThread::DO_RENAME);
    copier->Run();
}

void CopyPage::OnCancel()
{

}

void CopyPage::OnKeyDown( wxKeyEvent& unused_arg )
{
	PathUtilDlg::DeleteSelections(m_copy_ext_list);
}

void CopyPage::Init( wxXmlNode * child )
{
    wxXmlNode* copy_page_child = child->GetChildren();
    while (copy_page_child)
    {
        // process text enclosed by <tag1></tag1>
        tchar_string node_name = copy_page_child->GetName().c_str();

        if (node_name == wxT("SourceDirectory"))
        {
            tchar_string content = copy_page_child->GetNodeContent().c_str();
            m_copy_source->SetValue(content);
        }
        else
            if (node_name == wxT("DestinationDirectory"))
            {
                tchar_string content = copy_page_child->GetNodeContent().c_str();
                m_copy_destination->SetValue(content);
            }
            else if(node_name==COPY_PAGE_INCLUDE_EXTENSIONS_NODE)
            {
                tchar_string content = copy_page_child->GetNodeContent().c_str();
                if (content==wxT("yes"))
                {
                    m_copy_ext_flag->SetSelection(0);
                }
                else
                {
                    m_copy_ext_flag->SetSelection(1);
                }
            }
            else if (node_name == wxT("AddTimestampToFileName"))
            {
                tchar_string content = copy_page_child->GetNodeContent().c_str();
                m_add_timestamp->SetValue(content==wxT("yes"));
            }
            else if (node_name == wxT("OperationType"))
            {
                tchar_string content = copy_page_child->GetNodeContent().c_str();
                m_operation_type->SetValue(content);
            }
            else
                PathUtilDlg::LoadNode(m_copy_ext_list, copy_page_child, wxT("extensions"), wxT("extension"));
            copy_page_child = copy_page_child->GetNext();
    }
}

wxXmlNode* CopyPage::GetNode()
{
    wxXmlNode* copy_page_node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("CopyPage"));

    wxXmlNode* copy_source = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("SourceDirectory"));
    copy_source->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_copy_source->GetValue()));
    copy_page_node->AddChild(copy_source);

    wxXmlNode* copy_dst = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("DestinationDirectory"));
    copy_dst->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_copy_destination->GetValue()));
    copy_page_node->AddChild(copy_dst);

    wxXmlNode* flag_node = new wxXmlNode(wxXML_ELEMENT_NODE, COPY_PAGE_INCLUDE_EXTENSIONS_NODE);
    copy_page_node->AddChild(flag_node);
    if(m_copy_ext_flag->GetSelection()==0)
    {
        flag_node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("unused_name"), wxT("yes")));
    }
    else
    {
        flag_node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("unused_name"), wxT("no")));
    }

    copy_page_node->AddChild( PathUtilDlg::GetNode(m_copy_ext_list, wxT("extensions"), wxT("extension")));

    wxXmlNode* add_tm = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AddTimestampToFileName"));
    add_tm->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_add_timestamp->IsChecked()? wxT("yes"):wxT("no")  ));
    copy_page_node->AddChild(add_tm);

    wxXmlNode* op_type = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("OperationType"));
    op_type->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), m_operation_type->GetValue()  ));
    copy_page_node->AddChild(op_type);

    return copy_page_node;
}

wxString CopyPage::GetName()
{
    wxString type_name =  wxConvLocal.cMB2WX(typeid(*this).name());
    int pos = type_name.find(_T(' '));
    if (pos>0)
    {
        return type_name.substr(pos+1);
    }
    return type_name;
}
