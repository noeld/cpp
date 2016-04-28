//
//  untitled
//
//  Created by Arnold Müller on 2016-02-11.
//  Copyright (c) 2016 __MyCompanyName__. All rights reserved.
//
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o excepttest excepttest.cpp -lboost_system -lboost_filesystem -lboost_iostreams 

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <boost/filesystem.hpp>


namespace b = boost;
namespace bfs = boost::filesystem;

int main (int argc, char const *argv[])
{           
	using namespace std;
	int ret {0};
	try {
		cout << f1() << endl;
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception" << std::endl;
	}
	return ret;
}
