/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o rtree rtree.cpp -lboost_system -lboost_filesystem -lboost_iostreams  
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
# g++ -std=c++14 rtree.cpp -o rtree -O2 -s -Wall

/Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o rtree rtree.cpp
 */

#include <boost/format.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstdint>
#include <memory>
#include <utility>

using namespace std;
using boost::format;

namespace bg  = boost::geometry;
using point   = bg::model::point<int, 2, bg::cs::cartesian>;
using box     = bg::model::box<point>;
using rtval   = pair<box, string>;
using rtree_t = bg::index::rtree<rtval, bg::index::rstar<16>>;

struct ResultInfo {
	ResultInfo(string&& s, unsigned nr) 
		: resultinfo_(std::forward<std::string>(s)), nr_{nr} 
	{}
	string resultinfo_;
	unsigned nr_;
};

using shri  = shared_ptr<ResultInfo>;


struct KtoRange {
	unsigned i;
};

int main (int argc, char const *argv[])
{
	int ret = 0;
	try {
		rtree_t rtree;
		ifstream f("rtree_defs.txt");
		if (!(f.is_open() && f.good())) {
			throw "Konnte Datei nicht öffnen";
		}
		unsigned line_no {0};
		int xmin, ymin, xmax, ymax;
		string node_name;
		while(f >> xmin >> xmax >> ymin >> ymax >> node_name) {
			box b(point(xmin, ymin), point(xmax, ymax));
			rtree.insert(make_pair(b, node_name));
			++line_no;
			cout << format{"%3d: %-10s ((%d, %d), (%d, %d))"} 
						% line_no % node_name % xmin % ymin % xmax % ymax
				 << endl;
		}
		if (!f.eof()) {
			throw "Fehler beim Lesen";
		}
		cout << "Finished reading." << endl;
		vector<rtval> result;
		while(true) {
			int x, y;
			cout << "Enter x y: ";
			if (!(cin >> x >> y)) {
				cerr << "Exiting" << endl;
				break;
			}
			point p(x, y);
			result.clear();
			// rtree.query(bg::index::intersects(box(p, p)), std::back_inserter(result));
			rtree.query(bg::index::intersects(p), std::back_inserter(result));
			unsigned i = 0;
			cout << result.size() << " results found" << endl;
			for(const auto& r : result) {
				++i;
				cout << i << ": " << r.second << endl;
			}
		}
	} catch(const std::exception& e) {
		cerr << "std::exception: " << e.what() << endl;
	} catch(const char* e) {
		cerr << "const char*: " << e << endl;
	} catch(...) {
		cerr << "Unknown exception" << endl;
	}
	return ret;
}