//clang++ -L../libraries/libiconv-1.14/lib/ -liconv charset.cpp -o Testcharset --std=c++11
#include <iostream>
extern "C" {
#include "../libraries/libiconv-1.14/include/iconv.h"
}
#include <string>
#include <vector>

class Converter
{
public:
	Converter (std::string from, std::string to)	// type conversion
	: from_(from), to_(to), cd_{iconv_open(from_.c_str(), to_.c_str())} {
		if (reinterpret_cast<long int>(cd_) == -1) {
			throw("Fehler");
		}
	}
	Converter() = delete;									// default ctor
	Converter(const Converter&) =delete;							// copy ctor
	Converter(Converter&&) =delete;								// move ctor
	Converter& operator=(const Converter&) =delete;				// copy assignment
	Converter& operator=(Converter&&) =delete;					// move assignment
	virtual ~Converter () {
		iconv_close(cd_);
	}
	std::string convert(const std::string& from) {
		typedef std::remove_const<decltype(from[0])> myt;
		size_t tolen = static_cast<size_t>(from.length()*1.5);
		myt *to = new myt[tolen];
		size_t inlen = from.length();
		size_t outlen = tolen;
		myt *inptr = from.c_str();
		size_t ret = iconv(cd_, &inptr, &inlen, &to, &outlen);
//		tmp.resize(
		return std::string(to);
	}
private:
	std::string from_, to_;
	iconv_t cd_;
};

using namespace std;

int main (int argc, char const *argv[])
{
	vector<string> v(argv, argv+argc);
	int i{0};
	for(const auto& s : v) {
		cout << i++ << ": " << s << endl;
	}
	if (argc > 2) {
		Converter c{v[1], v[2]};
	}
	string line;
	do {
		getline(cin, line);
	} while(line.length() == 0);
	return 0;
}