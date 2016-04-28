/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o mandel mandel.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
 g++ -std=c++14 png.cpp -o png -O2 -s -Wall
 */

#include <iostream>
#include <string>

namespace PNG {

extern "C" {
	#include <stdio.h>
	#include "png.h"
	#include "zlib.h"
}

class PNGImage
{
public:
	PNGImage();
	~PNGImage() {
		cleanup();
	}
private:
	void init(const char* filename) {
		f_ = fopen(filename, "wb");
		if (nullptr == f_)
			throw "Cannot open file";
		png_ptr_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (nullptr == png_ptr_)
			throw
	}
	void cleanup() {
		if (nullptr != png_ptr_) {
			png_destroy_write_struct(&png_ptr_, &info_ptr_);
			png_ptr_ = nullptr;
			info_ptr_ = nullptr;
		}
		if (nullptr != f_) {
			fclose(f_);
			f_ = nullptr;
		}
	}
private:
	FILE*			f_ 			{nullptr};
	png_structp		png_ptr_ 	{nullptr};
	png_infop		info_ptr_	{nullptr}
};

}

 int main(int argc, char const *argv[])
 {
 	int ret = 0
 	try {

 	} catch(...) {
 		cout << "Unknown exception" << endl;
 	}
 	return ret;
 }