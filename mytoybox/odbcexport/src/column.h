/*
File created by Onega
*/

#ifndef COLUMN_H
#define COLUMN_H
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <sqlext.h>
#include <sql.h>
#include <iostream>
#include "strtypes.h"

typedef boost::shared_array<char> SmartBuffer;

class column
{
  public:
    column();
    
    tstring name;
    int size;
    int type;
    SQLINTEGER   data_size;
    int bind_type() const;    
    void clear_data();
    void set_size(int buffer_size);
    void alloc()
    {
      if (data_buf()==NULL && size>0)
	data.reset(new char[size]);
    }
    char* data_buf();
    SmartBuffer data;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        std::cout << __FUNCTION__ <<  std::endl;
	ar & name;
	ar & type;
	ar & size;	
    }
};
tostream& operator<<(tostream& os, column& col);

template<class Archive>
void save_data(Archive & ar, column& col){
    using boost::serialization::make_nvp;
    using boost::serialization::make_binary_object;
    ar << make_nvp("len", col.data_size);
    if (col.data_size>0)
      ar << make_nvp("data", make_binary_object(col.data_buf(), col.data_size));
}

template<class Archive>
void load_data(Archive & ar, column& col){
    using boost::serialization::make_nvp;
    using boost::serialization::make_binary_object;
    ar >> make_nvp("len", col.data_size);
    if ( col.size < col.data_size || col.data_buf()==NULL) 
    {
        col.set_size(col.data_size);
    }
    col.clear_data();
    if (col.data_size>0)
      ar >> make_nvp("data", make_binary_object(col.data_buf(), col.data_size));
}

typedef boost::shared_ptr<column> ptr_column;

#endif // COLUMN_H
