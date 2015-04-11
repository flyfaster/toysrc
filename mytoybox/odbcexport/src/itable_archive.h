/*
Created by Onega Zhang
*/

#ifndef ITABLE_ARCHIVE_H
#define ITABLE_ARCHIVE_H
#include <string>
#include "strtypes.h"

class itable_archive
{
public:
  itable_archive();;
  ~itable_archive();;
    int load_table_file(ODBCHandle& hDbc, const tstring& filename);
    
    
    void set_table(const tstring& table_name);
 
    int insert_table(ODBCHandle& hDbc, std::vector<column>& columns);
  private:
    tstring insert_sql;
    tstring table_name;
    tstring dsn_string;
};

#endif // ITABLE_ARCHIVE_H
