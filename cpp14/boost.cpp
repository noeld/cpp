#include <iostream>
#include <vector>
#include <boost/unordered_map.hpp>
#include <string>
#include <utility>
#include <boost/filesystem.hpp>

namespace b = boost;
namespace bfs = boost::filesystem;

using fstype = decltype(bfs::file_size(""));

struct Fileentry {
	bfs::path path_;
	std::string hash_;
	Fileentry(const bfs::path& p) : path_{p} {}
};

using svec = std::vector<Fileentry>;
using smap = b::unordered_map<fstype, svec>;

int main (int argc, char const *argv[])
{
	int ret {0};
	std::vector<std::string> params(argv, argv + argc);
	smap sm;
	for(const auto& f : params) {
		try {
			for(const auto& it : bfs::recursive_directory_iterator(f)) {
				if (bfs::is_directory(it)) 
					continue;
				b::system::error_code ec;
				auto fs = bfs::file_size(it, ec);
				if (!ec) {
					std::cout << fs << ": " << it << std::endl;
					auto f = sm.find(fs);
					if (f == sm.end()) {
						sm[fs] = svec;
					} else {
						f
					}
					
					sm.emplace_back(it);
				} else {
					std::cerr << "Error " << it << ": " << ec << std::endl;
				}
			}
		} catch(const bfs::filesystem_error& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	std::cout << files.size() << " Dateien gefunden." << std::endl;
	return ret;
}