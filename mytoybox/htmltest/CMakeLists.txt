project(htmltest)

add_executable(htmltest clock_timer.cpp resource_database.cpp page_parser.cpp url_class.cpp http_downloader.cpp main.cpp)

target_link_libraries (htmltest gumbo pthread boost_system boost_program_options boost_filesystem curl crypto sqlite3)
