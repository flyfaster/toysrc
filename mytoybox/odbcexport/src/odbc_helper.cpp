
#include "boost/date_time/local_time/local_time.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/binary_object.hpp>
#include "odbc_helper.h"
#include <sstream>
#include <string.h>
#include <iostream>
#include <wx/wx.h>
std::string SqlTypeToString(SQLSMALLINT type)
{
    switch(type)
    {
    case SQL_CHAR:   return "SQL_CHAR";
    case SQL_VARCHAR:  return "SQL_VARCHAR";
    case SQL_LONGVARCHAR:  return "SQL_LONGVARCHAR";
    case SQL_WCHAR:    return "SQL_WCHAR";
    case SQL_WVARCHAR:   return "SQL_WVARCHAR";
    case SQL_WLONGVARCHAR:    return "SQL_WLONGVARCHAR";
    case SQL_DECIMAL:        return "SQL_DECIMAL";
    case SQL_NUMERIC:        return "SQL_NUMERIC";
    case SQL_SMALLINT:        return "SQL_SMALLINT";
    case SQL_INTEGER:        return "SQL_INTEGER";
    case SQL_REAL:        return "SQL_REAL";
    case SQL_FLOAT:        return "SQL_FLOAT";
    case SQL_DOUBLE:        return "SQL_DOUBLE";
    case SQL_BIT:        return "SQL_BIT";
    case SQL_TINYINT:        return "SQL_TINYINT";
    case SQL_BIGINT:        return "SQL_BIGINT";
    case SQL_BINARY:        return "SQL_BINARY";
    case SQL_VARBINARY:        return "SQL_VARBINARY";
    case SQL_LONGVARBINARY:        return "SQL_LONGVARBINARY";
    case SQL_TYPE_DATE:        return "SQL_TYPE_DATE";
    case SQL_TYPE_TIME:        return "SQL_TYPE_TIME";
    case SQL_TYPE_TIMESTAMP:        return "SQL_TYPE_TIMESTAMP";
    //case SQL_TYPE_UTCDATETIME:    return "SQL_TYPE_UTCDATETIME";
    //case SQL_TYPE_UTCTIME:        return "SQL_TYPE_UTCTIME";
    case SQL_GUID:        return "SQL_GUID";
    }
    std::stringstream ss;
    ss<<"unknown type:"<<(long)type;
    return ss.str();
}
SQLRETURN ODBCEnv::Init() throw (odbc_exception) {
    SQLRETURN rc = SQL_SUCCESS;
    rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (!SQL_SUCCEEDED(rc))
      throw odbc_exception(__FUNCTION__, __LINE__, "Allocate env failed.");
    rc = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    if (!SQL_SUCCEEDED(rc))
      throw odbc_exception(__FUNCTION__, __LINE__, "Set env failed.");
    return rc;
}
ODBCEnv::~ODBCEnv() {
    if (hEnv)
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    hEnv = NULL;
}
ODBCEnv::ODBCEnv() : hEnv(NULL)
{

}
ODBCHandle::~ODBCHandle() {
    if (hDbc)
    {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    }
    hDbc = NULL;
}
SQLRETURN ODBCHandle::Init() {
    SQLRETURN rc = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    return rc;
}
SQLRETURN ODBCHandle::Connect( SQLHWND hWnd, const tstring& szInConnStr, SQLUSMALLINT fDriverCompletion )
{
    SQLTCHAR szConnStrln[1024]={0};
    memset(szConnStrln, 0, sizeof(szConnStrln));
    std::copy(szInConnStr.begin(), szInConnStr.end(), szConnStrln);
//     for (size_t i=0; i<szInConnStr.length(); i++)
//       szConnStrln[i] = szInConnStr[i];
    SQLTCHAR szOutConnStr[1024]={0};
    SQLSMALLINT dwOutConnStrLen = sizeof(szOutConnStr);
    SQLRETURN rc=  SQLDriverConnect(hDbc, hWnd,
                                    szConnStrln, //reinterpret_cast<SQLTCHAR*> (const_cast<TCHAR*>(szInConnStr.c_str())),
				    szInConnStr.length(),
                                    reinterpret_cast<SQLTCHAR*>(szOutConnStr), dwOutConnStrLen,
                                    &dwOutConnStrLen,  fDriverCompletion);
	OutConnectionString = sqlstr2tstr<tstring>(szOutConnStr) ;

    if (!SQL_SUCCEEDED(rc))
    {
        RETCODE            rcError=0;
	SQLTCHAR            buffer[SQL_MAX_MESSAGE_LENGTH + 1]={0};
	SQLTCHAR            sqlstate[SQL_SQLSTATE_SIZE + 1]={0};
	SQLINTEGER         sqlcode=0;
	SQLSMALLINT        length=0;

        rcError=SQLError(hEnv, hDbc, SQL_NULL_HSTMT, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length);
        std::cout << "\n **** ERROR *****\n         SQLSTATE: "<<sqlstr2tstr<std::string>(sqlstate) <<std::endl;
        std::cout << "error: " << sqlstr2tstr<std::string>(buffer) << std::endl << "Native error code: "<<sqlcode<<std::endl;
        return rc;
    }
    return rc;
}
odbc_exception::odbc_exception(const std::string& position_hint, int line_no, const std::string& description) {
    this->position_hint=position_hint;
    this->line_no=line_no;
    this->description=description;
}
const char* odbc_exception::what() const throw(){
    return description.c_str();
}

odbc_exception::~odbc_exception() throw() {}

SQLRETURN ODBCStmt::Init(SQLHDBC hDbc)  throw (odbc_exception) {
    rc= SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (!SQL_SUCCEEDED(rc))
    {
        throw odbc_exception(__FUNCTION__, __LINE__, "failed to allocate statement");
    }
    return rc;
}

SQLRETURN ODBCStmt::Prepare(const tstring& sql_statement) throw(odbc_exception)
{
    SQLTCHAR sql_stat_buf[4096]={0};
    std::copy(sql_statement.begin(), sql_statement.end(), sql_stat_buf);
    rc = SQLCloseCursor(hStmt);
    rc = SQLPrepare(hStmt, sql_stat_buf, SQL_NTS);
    if (!SQL_SUCCEEDED(rc))
    {
      tstringstream ss;
      ss << _T("Failed to prepare statement ") << sql_statement;
        throw odbc_exception(__FUNCTION__, __LINE__, tstr2str(ss.str()));
    }
    return rc;
}
float strtohextoval(SQL_NUMERIC_STRUCT* sql_num_t) {
    float value=0;
    int i=1,last=1,current;
    int a=0,b=0;
    float divisor = 10;

    for (i=0;i<=15;i++)
    {
        current = (int) (*sql_num_t).val[i];
        a= current % 16; //Obtain LSD
        b= current / 16; //Obtain MSD

        value += last* a;
        last = last * 16;
        value += last* b;
        last = last * 16;
    }
    if (sql_num_t->sign == 0)
    {
        value *= -1;
    }
    for (i=0; i<sql_num_t->scale; i++)
        value/=divisor;
    return value;
}
std::string ODBCStmt::GetDiagRec() {
    std::stringstream ss;
    SQLSMALLINT count = 0;
    SQLSMALLINT messageLength = 0;
    SQLTCHAR sqlState[SQL_SQLSTATE_SIZE + 1]={0};
    SQLTCHAR message[SQL_MAX_MESSAGE_LENGTH+1]={0};
    SQLINTEGER nativeError;
    while (SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT,
                                       hStmt,
                                       ++count,
                                       sqlState,
                                       &nativeError,
                                       message,
                                       sizeof(message),
                                       &messageLength)))
    {
        ss << sqlState << " " << message << std::endl;
    }
    return ss.str();
}
SQLRETURN ODBCStmt::Tables(std::deque<std::string>& table_list) {
  table_list.clear();
    
    rc =  SQLTables( hStmt, 
		     (SQLTCHAR*)_T("%"), SQL_NTS, //catalog
		     (SQLTCHAR*)_T("%"), SQL_NTS, // schema
		     (SQLTCHAR*)_T("%"), SQL_NTS, // table
		     (SQLTCHAR*)_T("%"), SQL_NTS ); //table type

    if ( !SQL_SUCCEEDED(rc) )
    {
      std::cout << __FUNCTION__ << " error " << rc << std::endl;
      return rc;
    }
    
    SQLSMALLINT columns = 0;
    rc = SQLNumResultCols(hStmt, &columns);
    if ( !SQL_SUCCEEDED(rc) )
    {
      std::cout << __FUNCTION__ << " error " << rc << std::endl;
      return rc;
    }
    std::vector<column> table_columns(columns+1);
    rc = Columns(table_columns);
    /* Loop through the rows in the result-set */
    std::cout << __FUNCTION__ << " now navigate records\n";
    int row=0;
    while (SQL_SUCCEEDED(rc = SQLFetch(hStmt))) {        
	std::cout << "Row " << row++ << " \t";    
        for (SQLUSMALLINT i = 1; i <= columns; i++) {
            SQLINTEGER indicator;
            char buf[512]={0};
            /* retrieve column data as a string */
            rc = SQLGetData(hStmt, i, SQL_C_CHAR,
                            buf, sizeof(buf), &indicator);
            if (SQL_SUCCEEDED(rc)) {
                /* Handle null columns */
                if (indicator == SQL_NULL_DATA) 
		  strcpy(buf, "NULL");
		std::cout <<  buf <<" \t";
            }
            if (i==3)
	      table_list.push_back(buf);
        }
        std::cout << std::endl;
    }
  std::cout << __FUNCTION__ << " code " << rc << std::endl;
  return rc;
}
SQLRETURN ODBCStmt::Columns(std::vector< column >& columns) {
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

    if (columns.size()!=nCols+1)
        columns.resize(nCols+1);

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

        std::cout<< sqlstr2tstr<std::string>(szHeader) <<" type: "<<SqlTypeToString(swColType)
                 <<"  column size:"<<(long)udwColDef <<std::endl;
        columns[nColCount].data_size = udwColDef;
    }
    return rc;
}


// std::string to_rfc3339(boost::posix_time::ptime t)
// {
//     using namespace boost::posix_time;
//     std::stringstream ss;
//     time_facet* lf(new time_facet("%Y-%m-%dT%H:%M:%S%F%Q"));
//     ss.imbue(std::locale(std::locale::classic(), lf));
//     ss << t ;
//     return ss.str();
// }
// http://www.oracle.com/technology/docs/tech/windows/odbc/htdocs/817help/sqoraUnicode_Support.htm