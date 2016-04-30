/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o join join.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
# g++ -std=c++14 join.cpp -o join -O2 -s -Wall

/Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o join join.cpp
 */
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <unordered_map>
#include <boost/tokenizer.hpp>

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

private:
	std::vector<Row> rows_;
	std::string name_;
	std::unordered_map<std::string, unsigned> schema_;
};

int main(int argc, char const *argv[])
{
	int ret = 0;
	std::vector<std::string> params(argv, argv + argc);
	std::vector<std::vector<std::string>> cols;
	using tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
	boost::escaped_list_separator<char> els('\\', ';', '"');
	try {
		std::ifstream in(params[1]);
		std::string buffer;
		tokenizer tok(buffer);
		std::cerr << "Reading " << params[1];
		unsigned lines = 0;
		if (in.good())
		{
			std::getline(in, buffer);
			tok.assign(buffer, els);
			for(const auto& s : tok) {
				std::cerr << s << std::endl;
				cols.emplace_back();
			}
			std::cerr << "Die Datei hat " << cols.size() << " Spalten." << std::endl;
		}
		while (in.good())
		{
			std::getline(in, buffer);
			if (buffer.size() == 0)
				continue;
			++lines;
			tok.assign(buffer);
			auto b = tok.begin();
			for(auto& s : cols) {
				if (b == tok.end()) {
					std::string s("Zeile " + std::to_string(lines) + " hat zu wenig Spalten");
					throw std::runtime_error(s);
				} else {
					s.emplace_back(*b);
					++b;
				}
			}
			if (b != tok.end()) {
				throw std::runtime_error(std::string("Zeile " + std::to_string(lines) + " hat zuviele Spalten."));
			}
		}
		std::cerr << "OK (" << lines << " lines read)" << std::endl;

		using map_t = std::unordered_map<std::string, unsigned>;
		using it_t = map_t::value_type;
		unsigned i = 0;
		for(const auto& v : cols) {
			++i;
			//std::ofstream of(std::string("join_") + std::to_string(i) + ".col");
			map_t map(v.size());
			for(const auto& s : v) {
				//of << s << std::endl;
				map[s]++;
			}
			std::cerr << std::to_string(i) << " hat " << v.size() << " Zeilen mit " << std::to_string(map.size()) << " verschiedenen Werten zwischen ";
			decltype(map.begin()) smin, smax;
			std::tie(smin, smax) = std::minmax_element(map.begin(), map.end()
				, [](const it_t& a, const it_t& b){
					return a.first < b.first;
				});
			std::cerr << smin->first << " und " << smax->first << std::endl;
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return ret;
}
