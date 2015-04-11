/*
Created by Onega Zhang
*/
#include <fstream>
#include <sqlext.h>
#include <iostream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <vector>
#include <sstream>
#include <wx/wx.h>
#include "column.h"
#include "odbc_helper.h"
#include "itable_archive.h"


int itable_archive::load_table_file(ODBCHandle& hDbc, const tstring& filename) 
{
  SQLLEN nInd = SQL_NULL_DATA;

    std::fstream ofs( tstr2str(filename).c_str(), std::ios_base::in|std::ios::binary);//, std::ios::binary
    if (ofs.bad())
    {
        tout << _T("Failed to open ") << filename << std::endl;
        return __LINE__;
    }

    boost::archive::binary_iarchive oa(ofs);
    SQLRETURN rc = 0;
    int column_count = 0;
    oa >> column_count;
    std::cout << __FUNCTION__ << " read column count " << column_count << std::endl;
    std::vector<column> columns(column_count+1);
    ODBCStmt hStmt;
    rc = hStmt.Init(hDbc);
    
    for (SQLSMALLINT i=1; i<=column_count; i++)
    {
        oa >> columns[i];
	    columns[i].alloc();
    }
    // load data

    insert_table(hDbc, columns);
    
    rc = hStmt.Prepare( insert_sql);    
    tout << _T("Prepare return ") << rc << _T(" for ") << insert_sql << std::endl;
    int record_count = 0;
    int field_count = 0;
    SQLCHAR tmpbuf[sizeof(double)];
    try
    {
        while (true)
        {
            for (SQLSMALLINT i=1; i<=column_count; i++)
            {
                load_data(oa, columns[i]);
                field_count++;
		tout << columns[i] << _T(" ");
		SQLSMALLINT fSqlType = columns[i].type;
		if (columns[i].type==SQL_FLOAT && columns[i].data_size>0 && isdigit(columns[i].data_buf()[0])==false)
		{
		  // money type is SQL_FLOAT, data is £1.50, need to remove leading non-digit character		
		  // money type should be bind as SQL_VARCHAR instead of SQL_FLOAT
		  memset(tmpbuf, 0, sizeof(tmpbuf));
		  int valid_str_len = columns[i].data_size-2;
		  memcpy(tmpbuf, columns[i].data_buf()+2, valid_str_len);
		  memcpy(columns[i].data_buf(), tmpbuf, columns[i].data_size);
		  fSqlType = SQL_VARCHAR;
		}		
		// http://www.sqldev.org/sql-server-data-access/how-to-insert-a-null-in-a-sqltypetimestamp-column-61926.shtml
                rc = SQLBindParameter(hStmt,
                                      i,
                                      SQL_PARAM_INPUT,
                                      columns[i].bind_type(), // SQL_C_CHAR, The C data type of the parameter.
                                      fSqlType, // The SQL data type of the parameter
                                      columns[i].size, // The size of the column or expression of the corresponding parameter marker
                                      0, // The decimal digits of the column or expression of the corresponding parameter marker.
                                      columns[i].data_buf(),
                                      columns[i].size, // Length of the ParameterValuePtr buffer in bytes
                                      &columns[i].data_size);

                if ( (rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO) )
                {
                    std::cout << hStmt.GetDiagRec();
                    return(__LINE__);
                }
            }            
            record_count++;
            std::cout << std::endl;
	    rc = SQLExecute(hStmt);
            if ( (rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO) ) 
	    {
                std::cout << "Insert " << record_count << " " << hStmt.GetDiagRec();
                return(__LINE__);
            }
        }
    }
    catch (boost::archive::archive_exception& e)
    {
        if (field_count % column_count)
            std::cout << typeid(e).name() << ": " << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << typeid(e).name() << " exception: " << e.what() << std::endl;
    }
    std::cout << "record " << record_count << std::endl;
    return 0;
}

void itable_archive::set_table( const tstring& table_name )
{
    this->table_name=table_name;
    
}

int itable_archive::insert_table(ODBCHandle& hDbc, std::vector< column >& columns) {
    if (table_name.length()==0)
        return __LINE__;
    if (columns.size()<2)
        return __LINE__;
    if (insert_sql.length()==0)
    {
        tstringstream ss;
        ss << _T("INSERT INTO ") << table_name << _T("( \"");
        for (size_t i=1; i<columns.size()-1; i++)
        {
            ss << columns[i].name << _T("\", \"");
        }
        ss << columns[columns.size()-1].name << _T("\")  VALUES( ");
        for (size_t i=1; i<columns.size()-1; i++)
        {
            ss << _T("?, ") ;
        }
        ss << _T("? )");
        insert_sql=ss.str();
    }
    
    return 0;
}
itable_archive::itable_archive() {}
itable_archive::~itable_archive() {}
