CFLAGS = -fno-omit-frame-pointer -std=c++11 -g -pthread
LDFLAGS = -rdynamic -lbfd -ldl

all: abstract_factory

abstract_factory : abstract_factory.cpp
	g++ ${CLFAGS} -o $@ $< ${LDFLAGS}
	
clean:
	rm 	abstract_factory