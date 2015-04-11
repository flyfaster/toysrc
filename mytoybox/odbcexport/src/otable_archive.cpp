/*
Created by Onega Zhang
reference - http://support.microsoft.com/kb/222831
*/
#include <iostream>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wx/wx.h>
#include "strtypes.h"
#include "otable_archive.h"
#include "odbc_helper.h"
#include "column.h"



//SQLRETURN otable_archive::ExportTable(ODBCHandle& hDbc, const std::string& table, const std::string& filename) {
//    RETCODE            rcError;
//    SQLCHAR            buffer[SQL_MAX_MESSAGE_LENGTH + 1];
//    SQLCHAR            sqlstate[SQL_SQLSTATE_SIZE + 1];
//    SQLINTEGER         sqlcode;
//    SQLSMALLINT        length;
//    ODBCStmt hStmt;
//    SQLRETURN rc = hStmt.Init(hDbc);
//    std::cout << "Allocate stmt return " << rc << std::endl;
//    char szStmt[1024];
//    if (table.length())
//    {
//        sprintf(szStmt, "SELECT * from %s", table.c_str() );
//    }
//    else
//    {
//        std::cout << "Table name is missing" << std::endl;
//        return __LINE__;
//    }
//
//
//    rc = SQLPrepare(hStmt,reinterpret_cast<SQLCHAR*>(szStmt), SQL_NTS);
//    std::cout << "Prepare return " << rc << " - " << szStmt << std::endl;
//    rc = SQLExecute(hStmt);
//    SQLSMALLINT nCols = 0;
//    const int MAX_DATA_LEN = 256;
//    char szHeader[MAX_DATA_LEN];
//    SQLSMALLINT swColLen;
//    SQLSMALLINT swColType = 0;
//    SQLUINTEGER udwColDef =0;
//    SQLSMALLINT swColScale;
//    SQLSMALLINT swColNull;
//    rc = SQLNumResultCols(hStmt, &nCols); // Get Num of Result Columns,
//    std::cout << "Columns: " << nCols << std::endl;
//    std::vector<column> columns(nCols+1);
//
//    remove(filename.c_str());
//    std::ofstream ofs(filename.c_str(), std::ios::binary|std::ios::out);
//    boost::archive::binary_oarchive oa(ofs);
//    int ntmp = nCols;
//    oa << ntmp;
//
//    for (int nColCount=1; nColCount<=nCols; nColCount++)
//    {
//        szHeader[0] = 0;
//        rc = SQLDescribeCol(hStmt, nColCount,
//                            reinterpret_cast<SQLCHAR*>(szHeader),
//                            MAX_DATA_LEN, &swColLen, &swColType, &udwColDef,
//                            &swColScale, &swColNull ); // Get the header/title of each column
//        SmartBuffer col_buf(new char[udwColDef+2]);
//        columns[nColCount].name = szHeader;
//        columns[nColCount].type = swColType;
//        columns[nColCount].size = udwColDef;
//        columns[nColCount].data = col_buf;
//        oa << columns[nColCount];
//        std::cout<<szHeader<<" type: "<<SqlTypeToString(swColType)
//                 <<"  column size:"<<(long)udwColDef <<std::endl;
//        columns[nColCount].data_size = udwColDef;
//// 	if (columns[nColCount].bind_type()==SQL_C_TYPE_DATE)
//// 	{
//// 	          rc = SQLBindCol(hStmt,
////                         nColCount,
////                         columns[nColCount].bind_type(),  // The identifier of the C data type of the *TargetValuePtr buffer
////                         columns[nColCount].data_buf(),
////                         udwColDef,
////                         &columns[nColCount].data_size );
//// 	}
//// 	else
//        rc = SQLBindCol(hStmt,
//                        nColCount,
//                        columns[nColCount].bind_type(),  // The identifier of the C data type of the *TargetValuePtr buffer
//                        columns[nColCount].data_buf(),
//                        udwColDef,
//                        &columns[nColCount].data_size );
//        if (!SQL_SUCCEEDED(rc))
//        {
//            std::cout << "Bind column " << columns[nColCount].name << " error " << rc << std::endl;
//            return rc;
//        }
//    }
//    //return 0; //    archive column definition
//
//    rc = SQLFetch(hStmt); // Fetch row data from the statement cursor.
//    if (!SQL_SUCCEEDED(rc))
//    {
//        if (rc == SQL_NO_DATA)
//        {
//            std::cout << "No data available." << std::endl;
//            return rc;
//        }
//        std::cout << "Fetch return " << rc << std::endl;
//        buffer[0] = 0;
//        rcError=SQLError(hDbc.GetEnv(), hDbc, SQL_NULL_HSTMT, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
//        std::cout << "\n **** ERROR *****\n         SQLSTATE: "<< sqlstate<<std::endl;
//        std::cout << "error: " << buffer << std::endl << "Native error code: "<<sqlcode<<std::endl;
//        return rc;
//    }
//    int record_count = 0;
//    while (SQL_SUCCESS==rc||SQL_SUCCESS_WITH_INFO==rc)
//    {
////         std::cout << "record " << record_count<<std::endl;
//        record_count++;
//        for (size_t col_index=1; col_index <= nCols; col_index++)
//        {
//            save_data(oa, columns[col_index]);
//            int column_type = columns[col_index].bind_type();
//            if (columns[col_index].data_size<=0)
//            {
//                std::cout << "<null> ";
//                columns[col_index].clear_data();
//                continue;
//            }
//            if (columns[col_index].bind_type() == SQL_C_CHAR)
//            {
//                std::cout << columns[col_index].data_buf() << "\t";
//            }
//            else if (SQL_INTEGER == column_type)
//            {
//                std::cout << (*((int*)columns[col_index].data_buf())) << " ";
//            }
//            else
//            {
//                std::cout << "type is " << column_type << " length " << columns[col_index].data_size;
//            }
//
//            columns[col_index].clear_data();
//        }
//        std::cout<<std::endl;
//        rc = SQLFetch(hStmt);
//    }
//
//    SQLCloseCursor(hStmt);
//    std::cout << "Record count: " << record_count<<std::endl;
//    return rc;
//}
SQLRETURN otable_archive::ExportTableWithNull(ODBCHandle& hDbc, const tstring& table, const tstring& filename) {
    RETCODE            rcError;
    SQLTCHAR            buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLTCHAR            sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER         sqlcode;
    SQLSMALLINT        length;
    ODBCStmt hStmt;
    rc = hStmt.Init(hDbc);
   
//     TCHAR szStmt[1024];
    //wxPrintf(
    wxString szStmt;
    szStmt.Printf( _T("SELECT * from %s"), table.c_str() );
//     _stprintf(szStmt, _T("SELECT * from %s"), table.c_str() );
    std::cout << szStmt << std::endl;
    rc = hStmt.Prepare(szStmt.c_str());
    std::cout << "Prepare return " << rc << std::endl;
    rc = SQLExecute(hStmt);
    SQLSMALLINT nCols = 0;
    const int MAX_DATA_LEN = 256;
    SQLTCHAR szHeader[MAX_DATA_LEN];
    SQLSMALLINT swColLen;
    SQLSMALLINT swColType = 0;
    SQLUINTEGER udwColDef =0;
    SQLSMALLINT swColScale;
    SQLSMALLINT swColNull;
    rc = SQLNumResultCols(hStmt, &nCols); // Get Num of Result Columns,
    std::cout << "Columns: " << nCols << std::endl;
    std::vector<column> columns(nCols+1);

    remove( tstr2str(filename).c_str());
    std::ofstream ofs( tstr2str(filename).c_str(), std::ios::binary|std::ios::out);
    boost::archive::binary_oarchive oa(ofs);
    int ntmp = nCols;
    oa << ntmp;

    for (int nColCount=1; nColCount<=nCols; nColCount++)
    {
        szHeader[0] = 0;
        rc = SQLDescribeCol(hStmt, nColCount,
                            reinterpret_cast<SQLTCHAR*>(szHeader),
                            MAX_DATA_LEN, &swColLen, &swColType, &udwColDef,
                            &swColScale, &swColNull ); // Get the header/title of each column
        SmartBuffer col_buf(new char[udwColDef+2]);
        columns[nColCount].name = sqlstr2tstr<tstring>(szHeader) ;
        columns[nColCount].type = swColType;
        columns[nColCount].size = udwColDef;
        columns[nColCount].data = col_buf;
        oa << columns[nColCount];
        std::cout<< sqlstr2tstr<std::string>(szHeader)<<" type: "<<SqlTypeToString(swColType)
                 <<"  column size:"<<(long)udwColDef <<std::endl;
        columns[nColCount].data_size = udwColDef;
    }
    rc = SQLFetch(hStmt); // Fetch row data from the statement cursor.
    if (!SQL_SUCCEEDED(rc))
    {
        if (rc == SQL_NO_DATA)
        {
            std::cout << "No data available." << std::endl;
            return rc;
        }
        std::cout << "Fetch return " << rc << std::endl;
        buffer[0] = 0;
        rcError=SQLError(hDbc.GetEnv(), hDbc, SQL_NULL_HSTMT, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
        std::cout << "\n **** ERROR *****\n         SQLSTATE: "<< sqlstate<<std::endl;
        std::cout << "error: " << buffer << std::endl << "Native error code: "<<sqlcode<<std::endl;
        return rc;
    }
    int record_count = 0;
    while (SQL_SUCCESS==rc||SQL_SUCCESS_WITH_INFO==rc)
    {
        record_count++;
        for (SQLSMALLINT col_index=1; col_index <= nCols; col_index++)
        {

                rc = SQLGetData(hStmt, col_index,
                                columns[col_index].bind_type(),
                                (SQLPOINTER)columns[col_index].data_buf(), columns[col_index].size, 
				&columns[col_index].data_size);
            tout << columns[col_index] << _T(" ");
            save_data(oa, columns[col_index]);
            columns[col_index].clear_data();
        }
        std::cout<<std::endl;
        rc = SQLFetch(hStmt);
    }

    SQLCloseCursor(hStmt);
    std::cout << "Record count: " << record_count<<std::endl;
    return rc;
}