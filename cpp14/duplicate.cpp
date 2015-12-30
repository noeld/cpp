#include <iostream>
#include <fstream>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <utility>
#include <exception>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>
#include "ThreadPool.h"

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

struct HashTask {
	std::atomic<ltype::size_type> current_{0};
	const ltype& list_;
	HashTask(const ltype& l) : list_{l} {}
};

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
		size_t files_size{0};
		for(const auto& p : params) {
			bfs::path pp(p);
			if (bfs::is_directory(pp)) {
				std::cerr << "Searching " << pp << std::endl;
				for(const auto& p : bfs::recursive_directory_iterator(pp)) {
					try {
						if (bfs::is_regular_file(p)) {
							auto fs = bfs::file_size(p);
							sizemap << SFile(p);
							++files_added;
							files_size += fs;
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
		//std::cerr << "Sizemap contains " << sizemap.size() << " entries." << std::endl << files_skipped << " files were skipped" << std::endl << files_added << " files were added." << std::endl;
		std::map<std::string, std::vector<const SFile*>> hm;
		std::atomic<size_t> processed{0};
		std::atomic<int> captureoutput{0};
		size_t digested{0};
		size_t digested_bytes{0};
		size_t save_potential{0};
		std::thread obs([&processed, &captureoutput, files_added]() {
			while(1) {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				size_t sp = processed;
				if (!captureoutput) {
					std::cerr << boost::format{"%5.2f"} % (sp/static_cast<float>(files_added)*100.0f) << "%\r";
				}
				if (sp == files_added)
					break;
			}
		});
		for(const auto& s : sizemap) {
			if (s.second.size() <= 1) {
				++processed;
				continue;
			}
			//std::cerr << s.first << ": " << s.second.size() << " entries" << std::endl;
			save_potential += s.first * (s.second.size()-1);
			hm.clear();
			for(const auto& f : s.second) {
				auto d = f.digest();
				++digested;
				digested_bytes += f.size_;
				auto it = hm.find(d);
				if (it == hm.end()) {
					hm[d] = decltype(hm)::value_type::second_type();
				}
				hm[d].push_back(&f);
				++processed;
			}
			for(const auto& e : hm) {
				if (e.second.size() <= 1)
					continue;
				captureoutput = 1;
				std::cout << "[" << s.first << ", " << e.first << "]" << " " << e.second.size() << " entries" << std::endl;
				size_t i = 0;
				for (const auto& f : e.second) {
					++i;
					std::cout << boost::format{"%2d %s"} % i % f->path_ << std::endl;
				}
				captureoutput = 0;
			}
		}
		obs.join();
		std::cerr << boost::format{"%10d files added"} % files_added << std::endl;
		std::cerr << boost::format{"%10d files skipped"} % files_skipped << std::endl;
		std::cerr << boost::format{"%10d Megabytes size alltogether"} % (files_size/1024/1024) << std::endl;
		std::cerr << boost::format{"%10d files hashed (%3.1f%%)"} % digested % (static_cast<float>(digested)/files_added*100.0f) << std::endl;
		std::cerr << boost::format{"%10d Megabytes hashed (%3.1f%%)"} % (digested_bytes/1024/1024) % (static_cast<float>(digested_bytes)/files_size*100.0f) << std::endl;
		std::cerr << boost::format{"%10d Megabytes save potential"} % (save_potential/1024/1024) << std::endl;
		// ThreadPool::ThreadPool tp(4);
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unknown exception" << std::endl;
	}
	return ret;
}