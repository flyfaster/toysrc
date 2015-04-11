#include <sql.h>	// yum install unixODBC-devel
#include <sqlext.h>
#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <deque>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <exception>
#include <stdarg.h>
#include <array>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_array.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer/timer.hpp>
//#define BOOST_ALL_DYN_LINK
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "odbctool.h"

odbc_deleter::odbc_deleter(SQLSMALLINT type) :
		type_(type) {
}
// void odbc_deleter::operator()(auto odbchandle) 
// {
//   SQLRETURN rc = SQLFreeHandle(type_, odbchandle);
//   BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " SQLFreeHandle " 
//     << type_to_string(type_) << " "
//     << std::hex << odbchandle << std::dec
//     <<" return " << rc;
// }

const char* odbc_deleter::type_to_string(SQLSMALLINT type) {
	switch (type) {
	case SQL_HANDLE_STMT:
		return "SQL_HANDLE_STMT";
	case SQL_HANDLE_DBC:
		return "SQL_HANDLE_DBC";
	case SQL_HANDLE_ENV:
		return "SQL_HANDLE_ENV";
	default:
		BOOST_LOG_TRIVIAL(error)<< __FUNCTION__ << " unknown type " << type << std::endl;
		return "unknown type";
	}
}

std::string SqlTypeToString(SQLSMALLINT type) {
	switch (type) {
	case SQL_CHAR:
		return "SQL_CHAR";
	case SQL_VARCHAR:
		return "SQL_VARCHAR";
	case SQL_LONGVARCHAR:
		return "SQL_LONGVARCHAR";
	case SQL_WCHAR:
		return "SQL_WCHAR";
	case SQL_WVARCHAR:
		return "SQL_WVARCHAR";
	case SQL_WLONGVARCHAR:
		return "SQL_WLONGVARCHAR";
	case SQL_DECIMAL:
		return "SQL_DECIMAL";
	case SQL_NUMERIC:
		return "SQL_NUMERIC";
	case SQL_SMALLINT:
		return "SQL_SMALLINT";
	case SQL_INTEGER:
		return "SQL_INTEGER";
	case SQL_REAL:
		return "SQL_REAL";
	case SQL_FLOAT:
		return "SQL_FLOAT";
	case SQL_DOUBLE:
		return "SQL_DOUBLE";
	case SQL_BIT:
		return "SQL_BIT";
	case SQL_TINYINT:
		return "SQL_TINYINT";
	case SQL_BIGINT:
		return "SQL_BIGINT";
	case SQL_BINARY:
		return "SQL_BINARY";
	case SQL_VARBINARY:
		return "SQL_VARBINARY";
	case SQL_LONGVARBINARY:
		return "SQL_LONGVARBINARY";
	case SQL_TYPE_DATE:
		return "SQL_TYPE_DATE";
	case SQL_TYPE_TIME:
		return "SQL_TYPE_TIME";
	case SQL_TYPE_TIMESTAMP:
		return "SQL_TYPE_TIMESTAMP";
		//case SQL_TYPE_UTCDATETIME:
		//    return "SQL_TYPE_UTCDATETIME";
		//case SQL_TYPE_UTCTIME:
		//    return "SQL_TYPE_UTCTIME";
	case SQL_GUID:
		return "SQL_GUID";
	}
	std::stringstream ss;
	ss << "unknown type:" << (long) type;
	return ss.str();
}

void extract_error(const char *fn, SQLHANDLE handle, SQLSMALLINT type) {
	SQLINTEGER i = 0;
	SQLINTEGER native;
	SQLCHAR state[7];
	SQLCHAR text[256];
	SQLSMALLINT len;
	SQLRETURN ret;
	BOOST_LOG_TRIVIAL(error)<< "Database driver reported the following diagnostics whilst running "
	<< fn;

	do {
		ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
				sizeof(text), &len);
		if (SQL_SUCCEEDED(ret)) {
			BOOST_LOG_TRIVIAL(error)<< state << ":" << i << ":" << native << ":" << text;
		}
	}
	while( ret == SQL_SUCCESS );
}

SQLRETURN exec_stmt(SQLHDBC hDbc, const std::string& stmt) {
	std::array<char, 65536> stmtbuf;
	SQLRETURN rc = SQL_SUCCESS;
	SQLHSTMT hStmt;
	if (stmt.length() < 1)
		return rc;

	rc = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	boost::shared_ptr<void> stmtholder(hStmt, odbc_deleter(SQL_HANDLE_STMT));
	strcpy(stmtbuf.data(), stmt.c_str());
	SQLPrepare(hStmt, reinterpret_cast<SQLCHAR*>(stmtbuf.data()), SQL_NTS);
	rc = SQLExecute(hStmt);
	if (ODBC_FAILED(rc)) {
		extract_error(stmtbuf.data(), hStmt, SQL_HANDLE_STMT);
		throw_error("SQLExecute return %d", rc);
	}
	return rc;
}

SQLRETURN exec_sql_query(SQLHDBC hDbc, const std::string& sql) {
	SQLRETURN rc = SQL_SUCCESS;
	SQLHSTMT hStmt;
	rc = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	boost::shared_ptr<void> stmtholder(hStmt, odbc_deleter(SQL_HANDLE_STMT));
	std::cout << __FUNCTION__ << " " << std::hex << hStmt << std::dec
			<< std::endl;
	char szStmt[1024];
	strcpy(szStmt, sql.c_str());
	SQLPrepare(hStmt, reinterpret_cast<SQLCHAR*>(szStmt), SQL_NTS);
	rc = SQLExecute(hStmt);
	if (ODBC_FAILED(rc)) {
		extract_error(szStmt, hStmt, SQL_HANDLE_STMT);
		throw_error("SQLExecute return %d", rc);
	}
	BOOST_LOG_TRIVIAL(debug)<< szStmt << " return " << rc << std::endl;
	//rc = SQLExecDirect(hStmt, reinterpret_cast<SQLCHAR*>(szStmt),SQL_NTS);
	SQLSMALLINT nCols = 0;
	const int MAX_DATA_LEN = 1024;
	char szHeader[256];
	SQLSMALLINT swColLen;
	SQLSMALLINT swColType = 0;
	SQLULEN udwColDef = 0; // SQLUINTEGER udwColDef =0;
	SQLSMALLINT swColScale;
	SQLSMALLINT swColNull;
	SQLCHAR szRowBuffer[1024];
	SQLLEN bindcollenret; //  SQLINTEGER bindcollenret;
	odbc_row_type odbc_row;
	rc = SQLNumResultCols(hStmt, &nCols); // Get Num of Result Columns,
	if (ODBC_FAILED(rc)) {
		extract_error("SQLNumResultCols", hStmt, SQL_HANDLE_STMT);
		throw_error("SQLNumResultCols return %d", rc);
	}
	BOOST_LOG_TRIVIAL(debug)<< "Column count " << nCols << std::endl;
	odbc_row.resize(nCols + 1);
	for (int nColCount = 1; nColCount <= nCols; nColCount++) {
		szHeader[0] = 0;
		rc = SQLDescribeCol(hStmt, nColCount,
				reinterpret_cast<SQLCHAR*>(szHeader), MAX_DATA_LEN, &swColLen,
				&swColType, &udwColDef, &swColScale, &swColNull); // Get the header/title of each column

		if (udwColDef <= 0) {
			BOOST_LOG_TRIVIAL(debug)<< "column " << nColCount << std::endl;
			continue;
		}
		odbc_row[nColCount].set_name(szHeader);
		odbc_row[nColCount].type = swColType;
		odbc_row[nColCount].length = udwColDef;

		rc = SQLBindCol(hStmt, nColCount, swColType,
				odbc_row[nColCount].get_data_buf(), udwColDef + 2,
				&bindcollenret);
		if (ODBC_FAILED(rc)) {
			extract_error("SQLBindCol", hStmt, SQL_HANDLE_STMT);
			throw_error("SQLBindCol %d return %d", nColCount, rc);
		}
	}
	BOOST_LOG_TRIVIAL(debug)<< "SQLBindCol finished\n";
	rc = SQLFetch(hStmt); // Fetch row data from the statement cursor.
	if (ODBC_FAILED(rc)) {
		extract_error("SQLFetch", hStmt, SQL_HANDLE_STMT);
		throw_error("SQLFetch return %d", rc);
	}
	BOOST_LOG_TRIVIAL(debug)<< "SQLFetch finished\n";
	for (int i = 1; i <= nCols; i++) {
		std::cout << odbc_row[i].get_name() << "\t";
	}
	std::cout << std::endl;

	while (!ODBC_FAILED(rc)) {
		for (int i = 1; i <= nCols; i++) {
			std::cout << odbc_row[i] << "\t";
			odbc_row[i].clear();
		}
		std::cout << std::endl;
		rc = SQLFetch(hStmt);
	}

	SQLCloseCursor(hStmt);
	return rc;
}

void* odbc_column::get_data_buf() {
	switch (type) {
	case SQL_C_CHAR:
	case SQL_VARCHAR:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
		return get_sdata();
		break;
	case SQL_INTEGER:
	case SQL_SMALLINT:
		return &ndata;
		break;
	case SQL_TYPE_DATE:
		return &ds;
		break;
	case SQL_TYPE_TIME:
		return &ts;
	case SQL_TYPE_TIMESTAMP:
		return &tss;
	case SQL_REAL:
		return &f;
	case SQL_FLOAT:
	case SQL_DOUBLE:
		return &d;
	default:
		return get_sdata();
		break;
	}
	return 0;
}

void odbc_column::clear() {
	sdata[0] = 0;
	ndata = 0;
	f = 0;
	d = 0;
	ds.year = 0;
	ds.month = 0;
	ds.day = 0;
	ts.hour = ts.minute = ts.second = 0;
	tss.year = tss.month = tss.day = tss.hour = tss.minute = tss.second = 0;
	tss.fraction = 0;
}

std::ostream& operator<<(std::ostream& os, odbc_column& oc) {
//  os << oc.get_name() << "\t= ";
	switch (oc.type) {
	case SQL_C_CHAR:
	case SQL_VARCHAR:
		os << oc.get_sdata();
		break;
	case SQL_INTEGER:
		os << oc.ndata;
		break;
	case SQL_REAL:
		os << oc.f;
		break;
	case SQL_FLOAT:
	case SQL_DOUBLE:
		os << oc.d;
		break;
	case SQL_TYPE_DATE:
		os << oc.ds.year << std::setw(2) << std::setfill('0') << oc.ds.month
				<< std::setw(2) << std::setfill('0') << oc.ds.day;
		break;
	default:
		os << SqlTypeToString(oc.type) << " unsupported";
		break;
	}
	return os;
}
