/*
Created by Onega Zhang
*/

#ifndef OTABLE_ARCHIVE_H
#define OTABLE_ARCHIVE_H
#include <string>
#include <sqlext.h>
#include <sql.h>

class ODBCHandle;
class otable_archive
{
public:
    otable_archive(){};
    ~otable_archive()
    {
    }
    //SQLRETURN ExportTable(ODBCHandle& hDbc, const std::string& table, const std::string& filename);
    SQLRETURN ExportTableWithNull(ODBCHandle& hDbc, const tstring& table, const tstring& filename);
    SQLRETURN rc;
};

#endif // OTABLE_ARCHIVE_H
