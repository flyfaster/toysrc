#ifndef __odbctool_h__
#define __odbctool_h__
inline bool ODBC_FAILED(SQLRETURN rc) { return (rc&(~1))!=0; }
std::string SqlTypeToString(SQLSMALLINT type);
SQLRETURN exec_stmt(SQLHDBC hDbc, const std::string& stmt);
SQLRETURN exec_sql_query(SQLHDBC hDbc, const std::string& sql);

void extract_error(const char *fn, SQLHANDLE handle, SQLSMALLINT type);
void throw_error ( const char * format, ... );

struct odbc_deleter
{
  odbc_deleter(SQLSMALLINT type);
    void operator()(auto odbchandle) 
{
  SQLRETURN rc = SQLFreeHandle(type_, odbchandle);
  BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " SQLFreeHandle " 
    << type_to_string(type_) << " "
    << std::hex << odbchandle << std::dec
    <<" return " << rc;
}    
    static const char* type_to_string(SQLSMALLINT type);

private:
  SQLSMALLINT type_;
};

struct odbc_column {
  std::array<char, 64> name;
  SQLSMALLINT type;
  SQLSMALLINT length;
  int ndata;
  float f;
  double d;
  SQL_DATE_STRUCT ds;
  SQL_TIME_STRUCT ts;
  SQL_TIMESTAMP_STRUCT tss;
  std::array<char, 1024> sdata;
    void set_name(const char* sss)
    {
      strcpy(name.data(), sss);
    }
    char* get_name()
    {
      return name.data();
    }
    void set_sdata(const char* sss)
    {
      strcpy(sdata.data(), sss);
    }
    char* get_sdata()
    {
      return sdata.data();
    }
    void* get_data_buf();

    void clear();

};

std::ostream& operator<<(std::ostream& os, odbc_column& oc);



typedef std::vector<odbc_column> odbc_row_type;

#endif