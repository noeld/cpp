/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14  -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2  -o parallel parallel.cpp
 */
#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
//#include <parallel/sort.h>
#include <experimental/algorithm>

using namespace std;

 int main(int argc, char const *argv[])
 {
 	int ret = 0;
 	try {
 		std::random_device rd;
 		std::uniform_int_distribution<unsigned> dist(0, 0x9fffffff);
 		//auto f = std::bind(dist, move(rd));
 		constexpr const std::vector<unsigned>::size_type s = 10000000;
 		std::vector<unsigned> v(s);
 		for(auto& e : v) {
 			e =  dist(rd);
 		}
 		std::experimental::parallel::sort(std::experimental::parallel::par, v.begin(), v.end());
 	} catch(exception& e) {
 		cerr << "Exception " << e. what() << endl;
 	} catch(const char* e) {
 		cerr << "const char* " << e << endl;
 	} catch(...) {
 		cerr << "Unknown exception" << endl;
 	}
 	return ret;
 }
