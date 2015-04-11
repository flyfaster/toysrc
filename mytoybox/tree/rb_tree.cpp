/*
 * rb_tree.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: onega
 */

#include <iostream>
#include <cstdlib>
#include <deque>
#include <algorithm>
#include "rb_tree.h"

int main(int argc, char* argv[]) {
	std::cout << argv[0] << " start\n";
	rb_tree<int, int> mytree;
	size_t range = 20;

	std::deque<int> srcdata;
	for (size_t i = 0; i < range / 2; i++) {
		int data = 0;
		data = rand() % range;
		if (std::find(srcdata.begin(), srcdata.end(), data) == srcdata.end()) {
			srcdata.push_back(data);
			mytree.put(data, data * 10);
		}
	}
	std::cout << "http://www.cprogramming.com/c++11/c++11-lambda-closures.html"
			<< std::endl;
	auto print_to_stdout =
			[](rb_tree<int, int>::rb_node& node) {std::cout<< "{" <<node.key<<"," <<node.value << "}   ";};
	mytree.traverse(print_to_stdout);
	std::cout << std::endl;
	for (auto data : srcdata) {
		if (!mytree.contains(data)) {
			std::cout << "missing " << data << std::endl;
			continue;
		}
		if (mytree.get(data) != data*10)
		{
			std::cout << "invalid data for key " << data << std::endl;
			continue;
		}
		mytree.remove(data);
		mytree.traverse(print_to_stdout);
		std::cout << std::endl;
		if (mytree.contains(data))
			std::cout << "failed to remove " << data << std::endl;
		else
			std::cout << "removed " << data << std::endl;
	}
	std::cout << argv[0] << " done\n";
}
