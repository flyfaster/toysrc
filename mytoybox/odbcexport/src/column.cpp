/*
File created by Onega
*/
#include "boost/date_time/local_time/local_time.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/binary_object.hpp>
#include <sqlext.h>
#include <algorithm>
#include <stdlib.h>
#include <wx/wx.h>
#include "column.h"

column::column()
{
    type=0;
    size=0;
    data_size=0;
}

int column::bind_type() const
{
    switch(type)
    {
      case SQL_VARCHAR:
      case SQL_LONGVARCHAR:
      case SQL_CHAR:
	return SQL_C_CHAR;
	
      case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_LONGVARBINARY:
	return SQL_C_CHAR;
	
      case SQL_TIMESTAMP:
      case SQL_TYPE_DATE:
// 	return SQL_C_CHAR;
// 	return SQL_C_TYPE_TIMESTAMP;
	return SQL_C_TYPE_DATE;
	
      case SQL_SMALLINT:
	return SQL_C_SHORT;
      case SQL_INTEGER:
	return SQL_C_LONG;
      case SQL_FLOAT:
	return SQL_C_CHAR;
/*	if (size==7) £1.50
	  return SQL_C_NUMERIC; // SQL_NUMERIC_STRUCT*/
	return SQL_C_DOUBLE;
      case SQL_REAL:
	return SQL_C_FLOAT;
    }
    return type;
}

void column::clear_data()
{
    memset(data.get(), 0, (std::min)(size, 10));
//     data_size = 0;
}

void column::set_size(int buf_size)
{
    size = buf_size;
    if (size>0)
    {
    data = SmartBuffer(new char[buf_size]);
    }
}

char* column::data_buf()
{
    return data.get();
}

tostream& operator<<(tostream& os, column& col)
{
  if (col.data_size<=0)
  {
    os << _T("<null>");
    return os;
  }
  SQLDOUBLE  dv=0;
  float fv=0;
  switch(col.bind_type())
  {
    case SQL_C_CHAR:
      os << col.data_buf();
      break;
    case SQL_C_LONG:
       os << (*((int*)col.data_buf()));
       break;
    case SQL_C_SHORT:
      os << (*((short*)col.data_buf()));
       break;
    case SQL_C_FLOAT:
      memcpy(&fv, col.data_buf(), sizeof(fv));
      os<<fv;
      //os << (*((float*)col.data_buf()));
      break;
    case SQL_C_DOUBLE:
      memcpy(&dv, col.data_buf(), (std::min)(col.size, (int)col.data_size));
      os << dv << _T("{")<<col.data_size<< _T("}");
      //os << (*((double*)col.data_buf()));
      break;
    case SQL_C_TYPE_DATE:
    {
      const SQL_DATE_STRUCT* ds=(const SQL_DATE_STRUCT*)col.data_buf();      
      os << ds->year << _T("-")<< std::setw(2) << std::setfill(_T('0')) << ds->month << _T("-")
      << std::setw(2) << std::setfill(_T('0')) << ds->day;
    }
      break;
    default:
      os << _T("<type ") <<  col.bind_type() << _T(">");
      break;
  }
  return os;
}