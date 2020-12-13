﻿// duplicates.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "duplicates.h"
#include<string>
#include<boost/filesystem.hpp>
#include<boost/format.hpp>
#include<boost/iostreams/device/mapped_file.hpp>
#include<optional>
#include<algorithm>
#include<iterator>
#include<execution>
#include"dependencies/xxhash.hpp"

#define _CRT_SECURE_NO_WARNINGS 1

namespace bi = boost::iostreams;
namespace fs = boost::filesystem;

struct FormatedTime {
	explicit FormatedTime(std::time_t const& t) : t_{ t } {}
	friend std::ostream& operator<<(std::ostream& o, FormatedTime const& ft) {
		char time_buffer[100];
		auto i = std::min(sizeof(time_buffer)
			, std::strftime(time_buffer, sizeof(time_buffer), "%F %T", std::localtime(&ft.t_))
		);
		time_buffer[i] = '\0';
		return o << time_buffer;
	}
private:
	std::time_t const& t_;
};

struct file_entry {
public:
	explicit file_entry(fs::directory_entry const & entry) 
		: path_{entry.path()}, size_{fs::file_size(entry)}
	{}
	auto size() const { return size_; }
	auto const& path() const { return path_; }
	auto last_write_time() { 
		if (!last_write_time_)
			last_write_time_ = fs::last_write_time(path_);
		return *last_write_time_; 
	}
	auto hash() {
		if (!hash_)
			hash_ = compute_hash();
		return *hash_;
	}
private:
	xxh::hash64_t compute_hash() {
		bi::mapped_file_source source;
		source.open<fs::wpath>(this->path());
		if (!source.is_open()) {
			std::cerr << "Cannot open " << path() << std::endl;
			throw std::runtime_error("Cannot open file");
		}
		xxh::hash_state64_t hash_stream;
		hash_stream.update(source.data(), size_);
		return hash_stream.digest();
	}
private:
	fs::wpath path_;
	uintmax_t size_;
	std::optional<xxh::hash64_t> hash_;
	std::optional<std::time_t> last_write_time_;
};

using vector_type = std::vector<file_entry>;
using iterator_type = vector_type::iterator;

auto find_files_in_dir(fs::wpath const& path, vector_type& file_vector, uintmax_t min_size = 1) {
	size_t found = 0, ignored = 0;
	if (!fs::is_directory(path)) {
		std::cerr << path << " is not a directory!" << std::endl;
	}
	else {
		std::cerr << "Searching " << path << std::endl;

		for (auto& e : fs::recursive_directory_iterator(path)) {
			++found; 
			if (fs::is_regular_file(e) && fs::file_size(e) >= min_size)
				file_vector.emplace_back(e);
			else ++ignored;
		}
	}
	return std::make_tuple(found, ignored);
}

int main(int argn, char* argv[])
{
	vector_type files;
	for (auto i = 1; i < argn; ++i) {
		fs::wpath path(argv[i]);
		auto [found, ignored] = find_files_in_dir(path, files);
		std::cerr << boost::format{
			"  %1$6d files found\n"
			"  %2$6d files ignored\n"
			"  %3$6d files added\n" } % found % ignored % (found - ignored) 
			<< std::endl;
	}

	std::cerr << "Found " << files.size() << " regular files" << std::endl;
	std::sort(std::execution::par_unseq, files.begin(), files.end()
		, [](auto const& a, auto const& b) { return a.size() > b.size(); }
	);
	for_each_adjacent_range(
		std::begin(files)
		, std::end(files)
		, [](vector_type::value_type const& f) { return f.size(); }
		, [](auto start, auto end, auto file_size) {
			// Files with same size
			size_t nr_of_files = std::distance(start, end);
			if (nr_of_files > 1) {
				// sort range start-end by hash (desc), file time (desc), path (asc)
				std::sort(start, end, [](auto& a, auto& b) { 
					auto const& ha = a.hash();
					auto const& hb = b.hash();
					auto const& ta = a.last_write_time();
					auto const& tb = b.last_write_time();
					auto const& pa = a.path();
					auto const& pb = b.path();
					return std::tie(ha, ta, pb) > std::tie(hb, tb, pa); 
					});
				for_each_adjacent_range(
					start
					, end
					, [](vector_type::value_type& f) { return f.hash(); }
					, [file_size](auto hstart, auto hend, auto hash) {
						// Files with same size and same hash are assumed to be identical
						// could resort to compare files byte-by-byte now
						size_t hnr_of_files = std::distance(hstart, hend);
						if (hnr_of_files > 1) {
							std::cout << boost::format{ "%1$3d files with hash %3$016x and size %2$d\n" } 
								% hnr_of_files % file_size % hash;
							std::for_each(hstart, hend, [hash, file_size](auto& e) {
								std::cout << '\t' << FormatedTime(e.last_write_time()) << " " << e.path() << '\n';
								}
							);
						}
					}
				);
			}
		}
	);
	
	return 0;
}
