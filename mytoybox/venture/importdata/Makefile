
CFLAGS=-std=c++14 -DBOOST_ALL_DYN_LINK `PKG_CONFIG_PATH=/usr/local/lib/pkgconfig pkg-config --cflags breakpad` 
LDFLAGS=-lodbc -lboost_program_options -lboost_log -lboost_log_setup -lboost_timer -lboost_thread -lboost_system `PKG_CONFIG_PATH=/usr/local/lib/pkgconfig pkg-config --libs breakpad breakpad-client`

all : importyahoo urldecode

importyahoo: importyahoo.cpp odbctool.cpp odbctool.h daytrend.cpp daytrend.h
	g++ ${CFLAGS} -g  -o $@   -fpermissive daytrend.cpp importyahoo.cpp odbctool.cpp $(LDFLAGS)
	echo "Usage $@ -c config.ini -i GOOG.csv -s GOOG"

urldecode: urldecode.cpp
	g++ ${CFLAGS} -g  -o $@   -fpermissive urldecode.cpp $(LDFLAGS)

clean:
	if [ -f importyahoo ]; then rm importyahoo fi
	if [ -f urldecode ]; then rm urldecode fi