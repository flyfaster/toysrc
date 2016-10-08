all: server client daytime7 daytime1

clean:
	rm -f server client

server: server.cpp
	g++ -o server -g -pthread -std=c++0x -DBOOST_ALL_DYN_LINK server.cpp -lboost_program_options -lboost_log -lboost_thread -lboost_system

client: client.cpp
	g++ -o client -g -pthread -std=c++0x -DBOOST_ALL_DYN_LINK client.cpp -lboost_program_options -lboost_log -lboost_thread -lboost_system

daytime7: daytime7.cpp
	g++ -o daytime7 -g -pthread -std=c++0x -DBOOST_ALL_DYN_LINK daytime7.cpp -lboost_log -lboost_thread -lboost_system

daytime1: daytime1.cpp
	g++ -o daytime1 -g -pthread -std=c++0x -DBOOST_ALL_DYN_LINK daytime1.cpp -lboost_log -lboost_thread -lboost_system
	