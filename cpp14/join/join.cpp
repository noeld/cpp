/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o join join.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
# g++ -std=c++14 join.cpp -o join -O2 -s -Wall
 */
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
 
 class Row
 {
 public:
 	explicit Row(const std::vector<std::string>& v) : values_(v) 
 	{}
 	~Row() 				= default;
 	Row(const Row&) 	= default;
 	Row(Row&& r) : values_(std::move(r.values_)) {}
 	Row& operator=(const Row&) = delete;

 	std::vector<std::string>::size_type size() const { return values_.size(); }
 	const std::string& operator[](std::vector<std::string>::size_type i) const { return values_[i]; }
 private:
 	const std::vector<std::string> values_;
 };

 class Table
 {
 public:
 	Table(const std::string& name);
 	~Table();
 	unsigned size() const { return rows_.size(); }
 	const Row& operator[](unsigned i) const { return rows_[i]; }
 	void 
 private:
 	std::vector<Row> rows_;
 	std::string name_;
 	std::map<std::string, unsigned> schema_;
 };

int main(int argc, char const *argv[])
{
	int ret = 0;
	std::vector<std::string> params(argv, argv + argc);
	std::vector<std::string> file;
	try {
		std::ifstream in(params[1]);
		std::string buffer;
		std::cerr << "Reading " << params[1];
		unsigned lines = 0;
		while (in.good())
		{
			std::getline(in, buffer);
			file.push_back(buffer);
			++lines;
		}
		std::cerr << "OK (" << lines << " lines read)" << std::endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return ret;
}
