/*
Created by Onega Zhang
*/
#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include "mainwnd.h"
#include "odbcexportapp.h"
#include "odbc_helper.h"
#include "column.h"
#include "itable_archive.h"
#include "otable_archive.h"

// http://www.dreamincode.net/forums/topic/125264-keyboard-shortcuts-without-a-menu/
BEGIN_EVENT_TABLE(MainWnd, wxFrame)
//         EVT_BUTTON(wxID_ANY, MainWnd::OnChooseFile)
        
END_EVENT_TABLE()



MainWnd::MainWnd() : wxFrame(NULL, wxID_ANY, wxT("ODBCExport"))
{
    wxStaticText *dsn_lable = new wxStaticText(this, wxID_ANY, _T("DSN/Connection String"));
    m_dsn_input = new wxComboBox(this, wxID_ANY);
    m_table_btn = new wxButton(this, wxID_ANY, _T("Table"));
    m_table_input = new wxComboBox(this, wxID_ANY);
    m_file_btn = new wxButton(this, wxID_ANY, wxT("Import/Export File"));
    m_file_input = new wxTextCtrl(this, wxID_ANY);

    m_export_radio = new wxRadioButton(this, wxID_ANY,
                                       wxT("Export"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);

    m_import_radio = new wxRadioButton(this, -1,
                                       wxT("Import"), wxDefaultPosition);

    m_start_btn = new wxButton(this, wxID_ANY, _T("Start"));
    m_cancel_btn = new wxButton(this, wxID_ANY, _T("Cancel"));

    m_exit_btn = new wxButton(this, wxID_ANY, _T("Exit"));
    m_export_radio->SetValue(true);
    m_import_radio->SetValue(false);

    m_file_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                        wxCommandEventHandler(MainWnd::OnChooseFile), NULL, this);
    m_start_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(MainWnd::OnStart), NULL, this);
    m_cancel_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(MainWnd::OnCancel), NULL, this);
    m_exit_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWnd::OnExit), NULL, this);
    m_table_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWnd::OnTable), NULL, this);			
    wxGridBagSizer* sizer = new wxGridBagSizer();
    int col = 0;
    int row = 0;
    sizer->Add(dsn_lable, wxGBPosition(row, col));
    col++;
    sizer->Add(m_dsn_input, wxGBPosition(row, col), wxGBSpan(1, 1), wxEXPAND | wxRIGHT );
    row++;
    col=0;
    sizer->Add(m_table_btn, wxGBPosition(row, col));
    col++;
    sizer->Add(m_table_input, wxGBPosition(row, col));
    row++;
    col=0;
    sizer->Add(m_file_btn, wxGBPosition(row, col));
    col++;
    sizer->Add(m_file_input, wxGBPosition(row,col), wxDefaultSpan, wxEXPAND | wxRIGHT);
    row++;
    col=0;
    sizer->Add(m_export_radio, wxGBPosition(row, col));
    col++;
    sizer->Add(m_import_radio, wxGBPosition(row,col));

    wxGridSizer* btnsizer = new wxGridSizer(3);

    btnsizer->Add(m_start_btn);
    btnsizer->Add(m_cancel_btn);
    btnsizer->Add(m_exit_btn);

    row++;
    col=0;
    sizer->Add(btnsizer, wxGBPosition(row, col), wxGBSpan(1, 2), wxEXPAND|wxRIGHT);
    
    sizer->AddGrowableCol(1);
    sizer->AddGrowableRow(row);
    SetSizer(sizer);
    sizer->SetSizeHints(this);

    if (wxGetApp().vm.count("table"))
    {
        m_table_input->SetValue( wxString::FromAscii(wxGetApp().vm["table"].as<std::string>().c_str())   );
    }

    if (wxGetApp().vm.count("dsn"))
    {
        m_dsn_input->SetValue( wxString::FromAscii(wxGetApp().vm["dsn"].as<std::string>().c_str())   );
    }

    if (wxGetApp().vm.count("file"))
    {
        m_file_input->SetValue(wxString::FromAscii(wxGetApp().vm["file"].as<std::string>().c_str()));
    }
    Centre(wxBOTH);    
    UpdateDSN();    
}

void MainWnd::OnChooseFile(wxCommandEvent & event)
{
    //http://wiki.wxwidgets.org/Writing_Your_First_Application-Common_Dialogs
    wxFileDialog *OpenDialog;
    if (m_export_radio->GetValue())
    {
        OpenDialog= new wxFileDialog(this, _T("SaveAs"), wxEmptyString, wxEmptyString, wxEmptyString,wxFD_SAVE);
    }
    else
    {
        OpenDialog= new wxFileDialog(this, _T("Select import file"), wxEmptyString, wxEmptyString,  _T("Any file (*)|*"),
                                     wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    }
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
    {
        m_file_input->SetValue( OpenDialog->GetPath() );
    }
    OpenDialog->Destroy();
}

void MainWnd::OnStart(wxCommandEvent& evt)
{
    std::cout << __FUNCTION__ << std::endl;
    // check dsn, table name, file name
    tstring dsn = convert(m_dsn_input->GetValue());
    tstring table = convert(m_table_input->GetValue());
    tstring filename = convert(m_file_input->GetValue());
    if (dsn.length()==0 || table.length()==0 || filename.length()==0)
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL,
                wxT("Missing settings"), wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();
        return;
    }
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    SQLRETURN rc = SQL_SUCCESS;
    ODBCEnv hEnv;
    hEnv.Init();
    ODBCHandle hDbc(hEnv);
    hDbc.Init();

    rc = hDbc.Connect(NULL, dsn, SQL_DRIVER_COMPLETE);
    std::cout << __FUNCTION__ << " connect to " << tstr2str(dsn) << " return " << rc << std::endl;
    std::cout << __FUNCTION__ << " szOutConnStr " << tstr2str(hDbc.GetOutConnectionString())   << std::endl;
    RETCODE            rcError;
    SQLTCHAR            buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLTCHAR            sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER         sqlcode;
    SQLSMALLINT        length;

    if (!SQL_SUCCEEDED(rc))
    {
        rcError=SQLError(hEnv, hDbc, SQL_NULL_HSTMT, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
        std::cout << "\n **** ERROR *****\n         SQLSTATE: "<< sqlstate<<std::endl;
        std::cout << "error: " << buffer << std::endl << "Native error code: "<<sqlcode<<std::endl;
        return;
    }
    ptime t0 = microsec_clock::local_time();
    std::cout << "Start at " <<t0<<std::endl;

    if (m_import_radio->GetValue())
    {
        itable_archive table_archive;
        table_archive.set_table(table);
        table_archive.load_table_file(hDbc, filename);
        return;
    }

    try
    {
	otable_archive ota;
	rc = ota.ExportTableWithNull(hDbc, table, filename);
    }
    catch(const odbc_exception& e)
    {
      
        wxMessageDialog *dial = new wxMessageDialog(NULL,
                wxString::FromAscii(e.what()), 
						    wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();
    }
    ptime t1 = microsec_clock::local_time();
    time_duration duration(t1 - t0);
    std::cout << "End at " <<t1 << ", duration: " << duration<<std::endl;
}

void MainWnd::OnCancel(wxCommandEvent& evt)
{
    std::cout << __FUNCTION__ << std::endl;
}

void MainWnd::OnExit(wxCommandEvent & evt)
{
    Close();
}

void MainWnd::OnTable(wxCommandEvent& evt)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    m_table_input->Clear();
    tstring dsn = convert(m_dsn_input->GetValue());
    SQLRETURN rc = SQL_SUCCESS;

    ptime t0 = microsec_clock::local_time();
    std::cout << "Start at " <<t0<<std::endl;
    try
    {
	ODBCEnv hEnv;
	hEnv.Init();
	ODBCHandle hDbc(hEnv);
	hDbc.Init();

	rc = hDbc.Connect(NULL, dsn, SQL_DRIVER_COMPLETE);
	std::cout << __FUNCTION__ << " connect to " << tstr2str(dsn) << " return " << rc << std::endl;
	std::cout << __FUNCTION__ << " szOutConnStr " << tstr2str(hDbc.GetOutConnectionString())  << std::endl;
	
        std::deque<std::string> table_list;
        ODBCStmt hStmt;
        hStmt.Init(hDbc);
        hStmt.Tables(table_list);
        for (std::deque<std::string>::const_iterator it=table_list.begin();
                it!=table_list.end();
                it++)
        {
            m_table_input->AppendString(wxString::FromAscii(it->c_str()));
        }
    }
    catch (const odbc_exception& e)
    {

        wxMessageDialog *dial = new wxMessageDialog(NULL,
                wxString::FromAscii(e.what()),
                wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();
        dial->Destroy();
    }
    ptime t1 = microsec_clock::local_time();
    time_duration duration(t1 - t0);
    std::cout << "End at " <<t1 << ", duration: " << duration<<std::endl;
}
void MainWnd::UpdateDSN() {
    ODBCEnv hEnv;
    hEnv.Init();
    SQLCHAR tmpDSN[256];
    SWORD tmpDSNLen = 255;
    SWORD ResultLen=0;
    SQLCHAR tmpDriver[256];
    SWORD tmpDriverLen = 255;
    SWORD ResultDescLen;
    RETCODE ReturnCode = SQL_SUCCESS;

    m_dsn_input->Clear();
    ReturnCode = ::SQLDataSourcesA(hEnv, SQL_FETCH_FIRST, tmpDSN, tmpDSNLen, &ResultLen, tmpDriver, tmpDriverLen, &ResultDescLen);
    std::cout << __FUNCTION__ << " return " << ReturnCode << std::endl;
    while (SQL_SUCCEEDED(ReturnCode) && ResultLen>0)
    {
        m_dsn_input->AppendString( wxString(wxT("DSN="))+ wxString::FromAscii( (char*)tmpDSN) + wxString(_T(";")));
        ReturnCode = ::SQLDataSourcesA(hEnv, SQL_FETCH_NEXT, tmpDSN, tmpDSNLen, &ResultLen, tmpDriver, tmpDriverLen, &ResultDescLen);
        std::cout << __FUNCTION__ << " return " << ReturnCode << std::endl;
    }
}
