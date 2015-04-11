#pragma once
//#include <loki/pimpl.h>
#include <string>
#include <boost/shared_ptr.hpp>

//EXPIMP_TEMPLATE  /*template class STOCKCLIENT_API*/  Loki::PimplOf<SqliteSelectCallback>::Owner;
//STOCKCLIENT_API Loki::PimplOf<http_client>::Owner;
//EXPIMP_TEMPLATE  template class STOCKCLIENT_API std::string;


class STOCKCLIENT_API SqliteSelectCallback //: private Loki::PimplOf<SqliteSelectCallback>::Owner
{
public:
	SqliteSelectCallback(void);
	~SqliteSelectCallback(void);
	static int Callback(void *p_this, int num_fields, char **p_fields, char **p_col_names);
	std::string GetFieldName(int i);
	std::string GetFieldValue(int i, const std::string& fieldname);
	std::string GetFieldValue(int i, int fieldid);
	size_t GetFieldCount();
	size_t GetRecordCount();
	struct PrivateSqliteSelectCallback* d;
};

typedef boost::shared_ptr<SqliteSelectCallback> shared_SqliteSelectCallback;

//EXPIMP_TEMPLATE  template class STOCKCLIENT_API Loki::ImplOf<SqliteSelectCallback>;
//
//EXPIMP_TEMPLATE  template class STOCKCLIENT_API Loki::ConstPropPtr<Loki::ImplOf<SqliteSelectCallback> >;
//
//EXPIMP_TEMPLATE  template class STOCKCLIENT_API Loki::Pimpl<Loki::ImplOf<SqliteSelectCallback>,
//Loki::ConstPropPtr<Loki::ImplOf<SqliteSelectCallback> > >;
//
//EXPIMP_TEMPLATE  template class STOCKCLIENT_API Loki::PimplOwner<Loki::ImplOf<SqliteSelectCallback>,
//Loki::ConstPropPtr<Loki::ImplOf<SqliteSelectCallback> > >;
