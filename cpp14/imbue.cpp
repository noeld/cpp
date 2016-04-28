/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o mandel mandel.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
 g++ -std=c++14 imbue.cpp -o imbue -O2 -s -Wall
 */

#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <locale>
using namespace std;

#define PS(x) { cerr << (#x) << ": " << sizeof(x) << endl; }

int main(int argc, char const *argv[])
{
	int ret = 0;
	try {
		PS(wchar_t);
		cout << "User-preferred locale setting is " << std::locale("").name().c_str() << '\n';
		ifstream f;
		if (argc > 1)
			f.imbue(std::locale(argv[1]));

	} catch(exception& e) {
		cerr << "exception " << e.what() << endl;
	} catch(const char* e) {
		cerr << "const char* " << e << endl;
	} catch(...) {
		cerr << "unknown exception" << endl;
	}
	return ret;
}