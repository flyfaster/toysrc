//============================================================================
// Name        : sqlite3viewer.cpp
// Author      : Onega
// Version     :
// Copyright   : Your copyright notice
// Description : Display a table in specified in sqlite3 database
//============================================================================
#include <wx/frame.h>
#include <wx/app.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <sqlite3/sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/null.hpp"
#include <vector>
#include <iostream>
#include "myapp.h"
using namespace std;

boost::iostreams::stream< boost::iostreams::null_sink > nullOstream( ( boost::iostreams::null_sink() ) );

std::string table_name = "sqlite_master";
std::string dbpath;
int char_display_size = 9;
int max_column_size = 500;
int window_width = 1280;
char main_wnd_title[128];
int verbose=0;

std::ostream& getostream() {
	if(verbose)
		return std::cout;
	return nullOstream; // another not so good option: std::cout.setstate(std::ios_base::badbit);
}
// LD_LIBRARY_PATH=/home/onega/src/wxWidgets-3.1.0/lib:/home/onega/src/SQLiteCpp/Debug/sqlite3:/home/onega/src/SQLiteCpp/Debug:/usr/local/lib workspace/cdt/sqliteviewer/Debug/sqliteviewer
// [onega@localhost ~]$ LD_LIBRARY_PATH=/home/onega/src/wxWidgets-3.1.0/lib:/home/onega/src/SQLiteCpp/Debug/sqlite3:/home/onega/src/SQLiteCpp/Debug:/usr/local/lib workspace/cdt/sqliteviewer/Debug/sqliteviewer --dbpath /home/onega/workspace/cdt/sqlitefun/Debug/sqlitefun.db3
//Building file: ../src/sqliteviewer.cpp
//Invoking: GCC C++ Compiler
//g++ -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -I/home/onega/src/wxWidgets-3.1.0/lib/wx/include/gtk3-unicode-3.1 -I/home/onega/src/SQLiteCpp/include -I/home/onega/src/SQLiteCpp -I/home/onega/src/wxWidgets-3.1.0/include -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"src/sqliteviewer.d" -MT"src/sqliteviewer.o" -o "src/sqliteviewer.o" "../src/sqliteviewer.cpp"
//Finished building: ../src/sqliteviewer.cpp
//
//Building target: sqliteviewer
//Invoking: GCC C++ Linker
//g++ -L/home/onega/src/wxWidgets-3.1.0/lib -L/home/onega/src/SQLiteCpp/Debug/sqlite3 -L/home/onega/src/SQLiteCpp/Debug -pthread -o "sqliteviewer"  ./src/sqliteviewer.o   -lSQLiteCpp -lboost_filesystem -lboost_program_options -lsqlite3 -lboost_system -lwx_gtk3u_adv-3.1 -lwx_gtk3u_core-3.1 -lwx_baseu-3.1 -ldl
//Finished building target: sqliteviewer

// todo: Detach a wxApp from the terminal - Linux way
// todo: sort by click on column header

struct SortInfoStruct{
    int col ;
    int ascending;
    wxListCtrl* listctrl;
};
int wxCALLBACK ListCompareFunction(long item1, long item2, long sortData)
{

        SortInfoStruct * SortInfo = (SortInfoStruct *) sortData;
        int column = SortInfo->col;
        bool direction = SortInfo->ascending;
        wxString string1, string2;
        wxListCtrl * sortList = SortInfo->listctrl;
        wxListItem list_item;
        list_item.SetId (item1);
        list_item.SetColumn (column);
        list_item.SetMask (wxLIST_MASK_TEXT);
        sortList->GetItem (list_item);

        string1 = list_item.GetText();

        list_item.SetId(item2);
        list_item.SetColumn (column);
        list_item.SetMask (wxLIST_MASK_TEXT);
        sortList->GetItem (list_item);

        string2 = list_item.GetText();
        getostream() <<item1 <<" vs " << item2 <<" dir "<<SortInfo->ascending<<" "<< string1 << " vs " << string2 << std::endl;
        if(string1.CmpNoCase(string2) < 0)
                return direction ? -1 : 1;
        else
        {
                if(string1.CmpNoCase(string2) > 0)
                        return direction ? 1 : -1;
        }
        return 0;
}

int wxCALLBACK MyCompareFunctionDESC(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
    if (item1 < item2)
        return 1;
    if (item1 > item2)
        return -1;

    return 0;
}

class MyFrame : public wxFrame
{
    wxListCtrl* m_item_list;
    std::vector<int> m_item_list_sort; // sort descending or ascending
    std::vector<std::string> column_names; // column title of list control
    std::vector<size_t> column_value_length; // max value width
public:
    void create_header_columns() {
    	for(size_t i=0; i<column_names.size(); i++) {
			wxListItem col0;
			col0.SetId(i);
			col0.SetText( column_names[i].c_str() );
			col0.SetWidth(std::min( (size_t)max_column_size, column_value_length[i]*char_display_size));
			m_item_list->InsertColumn(i, col0);
			m_item_list_sort.push_back(0);
    	}
    }

    MyFrame() : wxFrame(NULL, wxID_ANY, wxString::Format(wxT("SQLite Viewer PID %i"),getpid()), wxPoint(50,50), wxSize(window_width,600))
    {
        wxPanel* mainPane = new wxPanel(this);
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

        m_item_list = new wxListCtrl(mainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        m_item_list->Connect(wxEVT_LIST_COL_CLICK, (wxObjectEventFunction)&MyFrame::OnLabelClick, NULL, this);

    	try
    	{
    	    SQLite::Database    db(dbpath.c_str());
    	    std::stringstream ss;
    	    ss << "SELECT * FROM " << table_name << " LIMIT 20"; // only check first 20 records to figure out column title and width
    	    SQLite::Statement   query(db, ss.str());
    	    while (query.executeStep())
    	    {
    	    	for(int i=0; i<query.getColumnCount(); i++) {
    	    		getostream() << query.getColumn(i).getName() << "\t";
    	    		if(column_names.size()==(size_t)i) {
    	    			column_names.push_back(query.getColumn(i).getName());
    	    			column_value_length.push_back(strlen(query.getColumn(i).getName())); // sometimes field name length > field value
    	    		}
    	    		// find out max value length of each column
					column_value_length[i] = std::max(column_value_length[i], strlen(query.getColumn(i).getText()));
    	    	}
    	    }
    	}
    	catch (std::exception& e)
    	{
    	    std::cout << "exception: " << e.what() << std::endl;
    	}

    	create_header_columns();

    	try
    	{
    	    SQLite::Database    db(dbpath.c_str());
    	    std::stringstream ss;
    	    ss << "SELECT * FROM " << table_name;
    	    SQLite::Statement   query(db, ss.str());
    	    int n=0;
    	    while (query.executeStep())
    	    {
    	    	for(int i=0; i<query.getColumnCount(); i++) {
    	    		getostream() << query.getColumn(i) << "\t";
    	            wxListItem item;
    	            item.SetId(n);
    	            item.SetText( query.getColumn(i).getText() );
    	            item.SetData(n); // store row number, used by wxListCtrlCompare, but it does not work. maybe I should store actual text here
    	            if(i==0)
    	            	m_item_list->InsertItem( item );
    	            m_item_list->SetItem(n, i, query.getColumn(i).getText());
    	    	}
    	    	n++;
    	    	getostream() << std::endl;
    	    }
    	}
    	catch (std::exception& e)
    	{
    	    std::cout << "exception: " << e.what() << std::endl;
    	}

        sizer->Add(m_item_list,1, wxEXPAND | wxALL, 10);
        mainPane->SetSizer(sizer);
    }
	void OnLabelClick(wxListEvent& event) {
		SortInfoStruct m_sortInfo;
		m_sortInfo.col = (int) event.GetColumn();
		m_item_list_sort[m_sortInfo.col] += 1;
		m_sortInfo.ascending = (m_item_list_sort[m_sortInfo.col]%2);
		m_sortInfo.listctrl = m_item_list;
		getostream() << __func__ << " sort on column " << m_sortInfo.col
				<< std::endl;
//		m_item_list->SortItems(ListCompareFunction, (long) &m_sortInfo); // not working well

	}
};


bool MyApp::OnInit()
{
    boost::program_options::options_description desc("Allowed options");
	desc.add_options()("help,h", "produce help message")
	("dbpath", boost::program_options::value<std::string>(&dbpath),"sqlite3 database file path")
	("table_name", boost::program_options::value<std::string>(&table_name)->default_value(table_name),"table_name in specified sqlite3 database, default is sqlite_master")
	("char_display_size", boost::program_options::value<int>(&char_display_size)->default_value(char_display_size),"specify display size of single character")
	("max_column_size", boost::program_options::value<int>(&max_column_size)->default_value(max_column_size),"specify max display size of column")
	("verbose", boost::program_options::value<int>(&verbose)->default_value(verbose),"specify if debug message is shown, default 0 disable debug message")
	("window_width", boost::program_options::value<int>(&window_width)->default_value(window_width),"specify width of main window")
	  ;
	boost::program_options::variables_map vm;
	boost::program_options::store(
			boost::program_options::parse_command_line(wxGetApp().argc, (char**)wxGetApp().argv, desc), vm);
	boost::program_options::notify(vm);
	boost::filesystem::path sqlite3path(dbpath);
	if (boost::filesystem::exists(sqlite3path))   // does path p actually exist?
			{
		if (boost::filesystem::is_regular_file(sqlite3path)) // is path p a regular file?
			getostream() << sqlite3path << " size is " << file_size(sqlite3path)
					<< '\n';

		else if (boost::filesystem::is_directory(sqlite3path)) // is path p a directory?
			cout << sqlite3path << " is a directory\n";

		else
			cout << sqlite3path
					<< " exists, but is not a regular file or directory\n";
	} else {
		cout << sqlite3path << " does not exist\n";
		cout << desc << endl;
		return 0;
	}

	m_frame = new MyFrame();
	m_frame->Show();
	return true;
}

IMPLEMENT_APP(MyApp)
