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
#include <thread>
#include <future>
#include <chrono>

class CompressedCol
{
public:
	CompressedCol() = default;
	~CompressedCol() = default;
	// insert and return value index
	unsigned push_back(const std::string& value) {
		auto search = valueMap_.find(value);
		unsigned vi;
		if (search == valueMap_.end()) {
			valueMap_.insert({value, valueCounter_});
			values_.push_back(value);
			vi = valueCounter_;
			++valueCounter_;
		} else {
			vi = search->second;
		}
		rows_.push_back(vi);
		return vi;
	}
	const std::string& operator[](unsigned i) const { return values_[rows_[i]]; }
	bool isPKCandidate() const noexcept { return rows_.size() == values_.size(); }
	// bool isFKCandidateFor(const CompressedCol& c) const { }
//private:
	unsigned valueCounter_ {0};
	// map value to unique index
	std::unordered_map<std::string, unsigned> valueMap_;
	// values 
	std::vector<std::string> values_;
	// original value from row i = values_[rows_[i]]
	std::vector<unsigned> rows_;
};

class Table
{
public:
	explicit Table(const std::string& name, char ce = '\\', char cs = ';', char cq = '"')
		: name_{name}, els{ce, cs, cq} 
	{
		readCSV();
	}
	Table(Table&& t) 
		: name_{std::move(t.name_)}, schema_{std::move(t.schema_)}, cc_{std::move(t.cc_)}, els{std::move(t.els)}
	{}
	~Table() = default;
	unsigned size() const { return cc_[0].rows_.size(); }
	//const Row& operator[](unsigned i) const { return rows_[i]; }
	void readCSV() {
		std::ifstream in(name_);
		if (!in.is_open())
			throw std::runtime_error(std::string("Cannot open ") + name_);
		std::string buffer;
		tokenizer tok(buffer);
		std::cerr << "Reading " << name_ << std::endl;
		unsigned lines = 0;
		if (in.good())
		{
			std::getline(in, buffer);
			tok.assign(buffer, els);
			for(const auto& s : tok) {
				//std::cerr << s << std::endl;
				cc_.emplace_back();
				schema_.push_back(s);
			}
			//std::cerr << "Die Datei hat " << cc_.size() << " Spalten." << std::endl;
		}
		while (in.good())
		{
			std::getline(in, buffer);
			if (buffer.size() == 0)
				continue;
			++lines;
			tok.assign(buffer);
			auto b = tok.begin();
			//for(auto& s : cols) {
			for(auto& c : cc_) {
				if (b == tok.end()) {
					std::string s("Zeile " + std::to_string(lines) + " hat zu wenig Spalten in " + name_);
					throw std::runtime_error(s);
				} else {
					c.push_back(*b);
					++b;
				}
			}
			if (b != tok.end()) {
				throw std::runtime_error(std::string("Zeile " + std::to_string(lines) + " hat zuviele Spalten in " + name_));
			}
		}
		//std::cerr << "OK (" << lines << " lines read)" << std::endl;
		// using namespace std::literals;
		// std::this_thread::sleep_for(5s);	
	}
	const std::vector<CompressedCol>& columns() { return cc_; }
	const std::string name() const { return name_; }
private:
	std::string name_;
	std::vector<std::string> schema_;
	
	std::vector<CompressedCol> cc_;
	using tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
	boost::escaped_list_separator<char> els{'\\', ';', '"'};
};

int main(int argc, char const *argv[])
{
	int ret = 0;
	std::vector<std::string> params(argv + 1, argv + argc);
	std::vector<std::unique_ptr<Table>> tables;
	try {
		std::vector<std::future<std::unique_ptr<Table>>> futures;
		for(const auto& name : params) {
			futures.emplace_back(async(std::launch::async, [name](){
				return std::unique_ptr<Table>(new Table(name));
			}));
		}
		for(auto& f : futures) {
			tables.push_back(f.get());
		}
		//tables.emplace_back(f);
		for(const auto& t : tables) {
			unsigned i = 0;
			std::cerr << t->name() << std::endl;
			for(const auto& v : t->columns()) {
				++i;
				std::cerr << std::to_string(i) << " hat " << v.rows_.size() << " Zeilen mit " << std::to_string(v.values_.size()) << " verschiedenen Werten zwischen ";
				decltype(v.values_.begin()) smin, smax;
				std::tie(smin, smax) = std::minmax_element(v.values_.begin(), v.values_.end());
				std::cerr << *smin << " und " << *smax << std::endl;
			}
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		ret = 1;
	}
	return ret;
}
