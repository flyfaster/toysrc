#include "stdafx.h"
#include <exception>
#include <string>
#include "db_exception.h"

db_exception::db_exception( int error_code, const char* error_message )
{
	if (error_message)
		m_description = error_message;
	m_error_code = error_code;
}

const char* db_exception::what() const
{
	return m_description.c_str();
}

int db_exception::error() const
{
	return m_error_code;
}
