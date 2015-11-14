/*
    Copyright (c) 2013 <copyright holder> <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PAGE_PARSER_H
#define PAGE_PARSER_H

class resource_database;
class page_parser {
public:
	page_parser();
	page_parser(const page_parser& other);
	virtual ~page_parser();
	virtual page_parser& operator=(const page_parser& other);
	virtual bool operator==(const page_parser& other) const;
	void set_resource_database(resource_database* db);
	resource_database* res_db_;
	void search_for_links(GumboNode* node);
	std::string get_page(const std::string& surl);
	std::string get_page2(const std::string& surl);
	std::string get_page3(const std::string& surl);
	void parse(const std::string& html_src);
	void set_depth(int d);
	int parse_page(const std::string& surl, int depth);
	void recursive(bool enable);
	int depth_;
	int max_depth_;
	url_class url_;
	bool recursive_;
	bool save_html_src_;
	std::fstream ss_;
	std::string get_temp_filename();
	std::string page_src_filename;

};


#endif // PAGE_PARSER_H
