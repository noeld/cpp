//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o sint sint.cpp -lboost_system -lboost_filesystem -lboost_iostreams 
#include <iostream>                                                
#include <boost/format.hpp>

using namespace std;
                       
class SInt {                        
public:
	static constexpr unsigned convert(int i)     { return (static_cast<unsigned>(i) << 1) ^ (i >> (sizeof(int)*8-1)); }
	static constexpr int convertBack(unsigned i) { return (i >> 1) ^ -static_cast<int>(i & 1); }
	explicit SInt(const int& i) : v_{convert(i)}{}
	operator unsigned() const noexcept { return v_; }
private:
	unsigned v_;
};

template<typename T>
struct Str2Bin {
	explicit Str2Bin(const T& t) : t_{t}{}
	const volatile T& t_;
};

template<typename T>
ostream& operator<<(ostream& o, const Str2Bin<T>& s) {
	T t = s.t_;
	for(unsigned i = 0; i < sizeof(T) * 8; ++i, t<<=1) {
		if (i>0 && i % 8 == 0)
			o << '.';
		o << (t & (1<<(sizeof(T)*8-1) ) ? '1' : '0');
	}
	return o;
}



int main (int argc, char const *argv[])
{
	int ret = 0;
	int max = 255;
	for(int i = -max; i <= max; ++i) {      
		SInt s(i);                                                                                         
		int j = SInt::convertBack(s);
		cout << boost::format{"%3d _ "} % i << Str2Bin<int>(i) 
			 << boost::format{" _ %3d _ "} % s 
			<< Str2Bin<unsigned>(s) << boost::format{" _ %3d _ "} % j 
			<< Str2Bin<int>(j) << endl;
	}
	return ret;
}