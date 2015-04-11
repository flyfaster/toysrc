#pragma once 

class STOCKCLIENT_API db_exception :public std::exception
{
public:
	db_exception(int error_code, const char* error_message);
	const char* what() const;
	int error() const;
private:
	int m_error_code;
	std::string m_description;
};
