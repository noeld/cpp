//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib -o mmfile mmfile.cpp -lboost_system -lboost_filesystem -lboost_iostreams -lcryptopp

#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <boost/iostreams/stream.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/program_options.hpp>

namespace b = boost;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

struct BaseTask {
	virtual const std::string& Name() const noexcept;
	virtual void Run(const std::vector<std::string>&)=0;
	virtual BaseTask& operator()(const std::vector<std::string>& p) final {
		Run(p);
		return *this;
	}
};

template<typename T>
struct Range {
	Range(T start, T end) : start_{start}, end_{end} {}
	T begin() const { return start_; }
	T end() const { return end_; }
private:
	mutable T start_;
	mutable T end_;
};

int main (int argc, char const *argv[])
{
	using namespace std;
	int ret{0};
	std::string infilename;
	try {
		bpo::options_description od{"Optionen"};
		od.add_options() 
			("help,h", "Help")
			("file,f", bpo::value<std::string>(&infilename), "Input file name")
			("chunksize,chunk,c", bpo::value<intmax_t>(), "Chunk size");
	} catch (const bpo::error& e) {
		std::cerr << e.what() << std::endl;
	}
	std::vector<std::string> params(argv, argv + argc);
	for (const auto& p : params) {
		std::cerr << p << std::endl;
	}
//	b::iostreams::stream<b::iostreams::mapped_file_source> file;
	b::iostreams::mapped_file_source src;
	if (params.size() > 1) {
		src.open(params[1]);
		cout << b::format{"%10d Size"} % src.size() << endl;
		auto a = b::iostreams::mapped_file_source::alignment();
		auto chunksize = (64 << 20) / a * a;
		cout << b::format{"%10d Alignment"} % b::iostreams::mapped_file_source::alignment() << endl;
		cout << b::format{"%10f Chunk size"} % (chunksize/1024.0/1024.0) << endl;
		size_t lines {0};
		//for(const char *it = src.data(), *end = src.data() + src.size(); it != end; ++it) {
		for(auto& it : Range<const char*>(src.data(), src.data() + src.size())) {	
			if (it == '\n')
				++lines;
		}
		cout << b::format{"%10d lines"} % lines << endl;
	}
	
	return ret;
}