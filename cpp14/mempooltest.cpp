//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib -o mempooltest mempooltest.cpp -lboost_system -lboost_filesystem -lboost_iostreams -lcryptopp

#include <iostream>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <complex>
#include <boost/format.hpp>
#include <utility>
#include <chrono>
//#include <ctime>
#include <ratio>

using namespace std;

struct tests {
	tests(int64_t i) : m_{i} {}
	~tests() = default;
	int64_t m_;
};

int main (int argc, char const *argv[])
{
	int ret = 0;
	try {
		{
			auto t_start = std::chrono::high_resolution_clock::now();
			{
				//vector<tests, boost::pool_allocator<tests>> vt;
				vector<tests> vt;
				for(size_t i = 0; i < 100000000; ++i) {
					vt.push_back(i);
				}
			}
			auto t_stop = std::chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::duration<double>>(t_stop - t_start);
			cout << boost::format{"Dauer: %2.6f s"} % duration.count() << endl;
		}
		{
			auto t_start = std::chrono::high_resolution_clock::now();
			{
				vector<tests, boost::pool_allocator<tests>> vt;
				//vector<tests> vt;
				for(size_t i = 0; i < 100000000; ++i) {
					vt.push_back(i);
				}
			}
			auto t_stop = std::chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::duration<double>>(t_stop - t_start);
			cout << boost::format{"Dauer: %2.6f s"} % duration.count() << endl;
		}
	} catch(const std::exception& e) {
		cerr << "std::exception: " << e.what() << endl;
	} catch(const char* e) {
		cerr << "const char: " << e << endl;
	} catch(...) {
		cerr << "unknown exception!" << endl;
	}
	return ret;
}
