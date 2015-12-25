#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <exception>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>

namespace b = boost;
namespace bfs = boost::filesystem;

using fstype = decltype(bfs::file_size(""));

struct SFile {
	const bfs::path path_;
	const fstype size_;
	SFile(const bfs::path& p) : path_{p}, size_(bfs::file_size(p)) {}
	auto size() const noexcept { return size_; }
	std::string digest() const {
		CryptoPP::SHA3_256 sha;
		CryptoPP::FileSource fs(path_.c_str(), false, new CryptoPP::HashFilter(sha));
		fs.Attach(new CryptoPP::HexEncoder());
		std::string result;
		fs.Attach(new CryptoPP::StringSink(result));
		fs.PumpAll();
		return result;
	}
};

using ltype = std::vector<SFile>;
using mstype = b::unordered_map<fstype, ltype>;

mstype& operator<<(mstype& m, const SFile& f) {
	auto s = f.size();
	auto it = m.find(s);
	if (it == m.end()) {
		m[s] = ltype();
	}
	m[s].push_back(f);
	return m;
}

int main (int argc, char const *argv[])
{
	int ret {0};
	std::vector<std::string> params(argv + 1, argv + argc);
	try {
		mstype sizemap;
		uint32_t files_added{0}, files_skipped{0};
		for(const auto& p : params) {
			bfs::path pp(p);
			if (bfs::is_directory(pp)) {
				std::cerr << "Searching " << pp << std::endl;
				for(const auto& p : bfs::recursive_directory_iterator(pp)) {
					try {
						if (bfs::is_regular_file(p) && bfs::file_size(p) > 0) {
							sizemap << SFile(p);
							++files_added;
							//std::cerr << "added" << std::endl;
						} else if (bfs::is_directory(p)) {
							std::cerr << "Searching " << p << std::endl;
						} else {
							++files_skipped;
							//std::cerr << "skipped" << std::endl;
						}
					} catch(bfs::filesystem_error& e) {
						std::cerr << "Error checking " << p << ": " << e.what() << std::endl;
					}
				}
			} else {
				sizemap << SFile(pp);
			}
		}
		std::cerr << "Sizemap contains " << sizemap.size() << " entries." << std::endl << files_skipped << " files were skipped" << std::endl << files_added << " files were added." << std::endl;
		std::map<std::string, std::vector<const SFile*>> hm;
		for(const auto& s : sizemap) {
			if (s.second.size() <= 1)
				continue;
			std::cerr << s.first << ": " << s.second.size() << " entries" << std::endl;
			hm.clear();
			for(const auto& f : s.second) {
				auto d = f.digest();
				auto it = hm.find(d);
				if (it == hm.end()) {
					hm[d] = decltype(hm)::value_type::second_type();
				}
				hm[d].push_back(&f);
			}
			for(const auto& e : hm) {
				if (e.second.size() <= 1)
					continue;
				std::cout << "[" << s.first << ", " << e.first << "]" << " " << e.second.size() << " entries" << std::endl;
				size_t i = 0;
				for (const auto& f : e.second) {
					++i;
					std::cout << i << "\t" << f->path_ << std::endl;
				}

			}
		}
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception" << std::endl;
	}

	return ret;
}