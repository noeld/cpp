/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o mandel mandel.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
 g++ -std=c++14 raii.cpp -o raii -O2 -s -Wall
 */
#include <iostream>
#include <memory>

using namespace std;

void del(int* i) { cout << "deleting int " << i << endl; delete &i; };

int main(int argc, char const *argv[])
{
	int ret = 0;

	// auto del = [](int&& i) { cout << "deleting int " << i << endl; delete i; };
	// unique_ptr<int> u(new int, del);
	unique_ptr<int> u(new int, del);



	return ret;
}