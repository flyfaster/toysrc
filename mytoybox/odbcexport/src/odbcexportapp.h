/*
Created by Onega Zhang
*/

#ifndef ODBCEXPORTAPP_H
#define ODBCEXPORTAPP_H
#include <wx/wx.h>

class ODBCExportApp: public wxApp
{
public:
  ODBCExportApp();
  virtual bool OnInit();
  boost::program_options::variables_map vm;
};

DECLARE_APP(ODBCExportApp)

void print_usage(const char* app_name,   boost::program_options::options_description& desc);


#endif // ODBCEXPORTAPP_H
