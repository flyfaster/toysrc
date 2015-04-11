CFLAGS=-g -pthread `wx-config --cppflags`
LDFLAGS= -lboost_filesystem -lboost_system `wx-config --libs base core aui`

all: storesym wxdumpsyms

storesym:main.cpp 
	g++ $(CFLAGS) -o $@ main.cpp ${LDFLAGS}
	
wxdumpsyms: wxdumpsyms.cpp MainWnd.cpp
	g++ $(CFLAGS) -o $@ wxdumpsyms.cpp MainWnd.cpp ${LDFLAGS} 	
	
clean:
	if [ -f storesym ]; then rm storesym; fi
	if [ -f wxdumpsyms ]; then rm wxdumpsyms; fi