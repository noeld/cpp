//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib -o mmfile mmfile.cpp -lboost_system -lboost_filesystem -lcryptopp

#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

int main (int argc, char const *argv[])
{
	int ret{0};
	
	return ret;
}