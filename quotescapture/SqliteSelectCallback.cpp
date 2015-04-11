#include "StdAfx.h"
#include <vector>
#include <deque>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include "sqlite3.h"
#include "SqliteSelectCallback.h"

struct from{};
struct to{};
using boost::multi_index_container;
using namespace boost::multi_index;

template<typename FromType,typename ToType>
struct bidirectional_map
{
	typedef std::pair<FromType,ToType> value_type;
	typedef multi_index_container<
		value_type,
		indexed_by<
		ordered_unique<
		tag<from>,member<value_type,FromType,&value_type::first> >,
		ordered_unique<
		tag<to>,  member<value_type,ToType,&value_type::second> >
		>
	> type;
};
typedef bidirectional_map<std::string,int>::type fieldname_id_map;

struct PrivateSqliteSelectCallback
//template<>
//struct Loki::ImplOf<SqliteSelectCallback>
{
	//ImplOf<SqliteSelectCallback>();
	PrivateSqliteSelectCallback(){}
	std::deque<std::string> m_values;
	fieldname_id_map m_fields;
	void AddField(const std::string& fieldname, int fieldindex);
};
//typedef Loki::ImplOf<SqliteSelectCallback> PrivateSqliteSelectCallback;
//
//PrivateSqliteSelectCallback::ImplOf()
//{
//
//}

void PrivateSqliteSelectCallback::AddField(const std::string& fieldname, int fieldindex)
{
	m_fields.insert(fieldname_id_map::value_type(fieldname, fieldindex));
}

SqliteSelectCallback::SqliteSelectCallback(void)
{
	d = new PrivateSqliteSelectCallback;
}

SqliteSelectCallback::~SqliteSelectCallback(void)
{
	if (d)
		delete d;
	d = NULL;
}

int SqliteSelectCallback::Callback( void *p_this, int num_fields, char **p_fields, char **p_col_names )
{
	SqliteSelectCallback* psc = reinterpret_cast<SqliteSelectCallback*>(p_this);
	if (psc->GetFieldCount()==0)
	{
		for (int i=0; i<num_fields; i++)
		{
			psc->d->AddField (p_col_names[i], i);
		}
	}
	for (int i=0; i<num_fields; i++)
	{
		if (p_fields[i])
			psc->d->m_values.push_back(p_fields[i]);
		else
			psc->d->m_values.push_back("");
	}
	return SQLITE_OK;
}

std::string SqliteSelectCallback::GetFieldName( int i )
{
	std::string fieldname;
	nth_index_iterator<fieldname_id_map,1>::type it2=get<1>(d->m_fields).find(i);
	if(it2!=get<1>(d->m_fields).end()){
		fieldname = it2->first;
	}
	return fieldname;
}

std::string SqliteSelectCallback::GetFieldValue( int i, const std::string& fieldname )
{
	int fieldid = -1;
	fieldname_id_map::iterator it=get<from>(d->m_fields).find(fieldname);
	if(it!=d->m_fields.end()){
		fieldid = it->second;
		return GetFieldValue(i, fieldid);
	}
	return std::string();
}

std::string SqliteSelectCallback::GetFieldValue( int i, int fieldid )
{
	std::string fieldvalue;
	if (fieldid < (int)GetFieldCount() && fieldid >= 0 && i>=0 && i<(int)GetRecordCount())
	{
		fieldvalue = d->m_values[(i)*GetFieldCount()+fieldid];

	}
	return fieldvalue;
}

size_t SqliteSelectCallback::GetFieldCount()
{
	return d->m_fields.size();
}

size_t SqliteSelectCallback::GetRecordCount()
{
	size_t count = 0;
	if (GetFieldCount())
		count = d->m_values.size()/GetFieldCount();
	return count;
}