all: quicksort heapsort mergesort

quicksort:quicksort.cpp
	g++ -o $@ -g -std=c++11 $< -lboost_timer -lboost_system

heapsort:heapsort.cpp
	g++ -o $@ -g -std=c++11 $<

mergesort:mergesort.cpp
	g++ -o $@ -g -std=c++11 $<

clean:
	rm -rf quicksort heapsort mergesort
