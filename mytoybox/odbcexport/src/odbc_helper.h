#ifndef __ODBC_HELPER__
#define __ODBC_HELPER__
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <exception>
#include <vector>
#include <deque>
#include "column.h"
#include "strtypes.h"

std::string SqlTypeToString(SQLSMALLINT type);
float strtohextoval(SQL_NUMERIC_STRUCT        *sql_num_t);

class odbc_exception : public std::exception
{
public:
    odbc_exception(const std::string& position_hint, int line_no, const std::string& description);
    virtual ~odbc_exception() throw();
    const char* what() const throw();
private:
    std::string position_hint;
    int line_no;
    std::string description;
};

class ODBCEnv
{
  public:
    ODBCEnv() ;
    SQLRETURN Init() throw (odbc_exception);
    ~ODBCEnv()    ;
    SQLHENV hEnv;
    operator SQLHENV()
    {
      return hEnv;
    }
};

class ODBCHandle
{
public:
    ODBCHandle(SQLHENV henv) : hDbc(NULL), hEnv(henv)     {
    }
    ~ODBCHandle() ;
    SQLRETURN Init() ;
    operator SQLHDBC()
    {
      return hDbc;
    }
    SQLRETURN Connect(SQLHWND hWnd, const tstring& szInConnStr, SQLUSMALLINT fDriverCompletion);
    tstring GetOutConnectionString()
    {
      return OutConnectionString;
    }
    SQLHENV GetEnv()
    {
      return hEnv;
    }
  private:
    SQLHDBC hDbc;
    SQLHENV hEnv;
    tstring OutConnectionString;
};

class ODBCStmt
{
  public:
    ODBCStmt() : hStmt(NULL)
    {
    }
    ~ODBCStmt()
    {
      if (hStmt)
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
      hStmt = NULL;
    }
    SQLRETURN Init(SQLHDBC hDbc) throw (odbc_exception);
    SQLRETURN Prepare(const tstring& sql_statement) throw (odbc_exception);
    std::string GetDiagRec();
    SQLRETURN Tables(std::deque<std::string>& table_list);
    SQLRETURN Columns(std::vector<column>& columns)    ;
    operator SQLHSTMT()
    {
      return hStmt;
    }
  private:
    SQLHSTMT hStmt;
    SQLRETURN rc;
};


#endif