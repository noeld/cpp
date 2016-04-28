/*
//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o mandel mandel.cpp -lboost_system -lboost_filesystem -lboost_iostreams -L/Users/arnold/Documents/prg/libraries/libpng-1.6.21/.libs 
# -lpng16 -I/Users/arnold/Documents/prg/libraries/libpng-1.6.21
 g++ -std=c++14 mandel.cpp -o mandel -O2 -s -Wall
 */
#include <iostream>
//#include <iomanip>
#include <fstream>
//#include <sstream>
//#include <iterator>
#include <array>
#include <string>
#include <vector>
//#include <unordered_map>
#include <exception>
#include <complex>
#include <functional>
#include <memory>
#include <thread>
#include <future>
#include <chrono>
#include <type_traits>
#include <numeric>
//#include <algorithm>
//#include <png.h>

using namespace std;

using cp_t = complex<double>;

template<typename T>
struct window {
	window(const T& xmin, const T& ymin, const T& xmax, const T& ymax) :
		xmin_{xmin}, ymin_{ymin}, xmax_{xmax}, ymax_{ymax}
	{
		if (xmin_ > xmax_)
			swap(xmin_, xmax_);
		if (ymin_ > ymax_)
			swap(ymin_, ymax_);
	}
	window() = default;
	window& operator=(const window&) = default;
	T width() const { return xmax_ - xmin_; }
	T height() const { return ymax_ - ymin_; }
	T xmin_ {static_cast<T>(-2.0)};
	T ymin_ {static_cast<T>(-1.5)};
	T xmax_ {static_cast<T>(1.0)};
	T ymax_ {static_cast<T>(1.5)};
	void zoom(double f) {
		double dx = (width()-width()*f)/2.0;
		double dy = (height()-height()*f)/2.0;
		xmin_ += dx;
		ymin_ += dy;
		xmax_ -= dx;
		ymax_ -= dy;
	}
	void move(double fx, double fy) {
		double dx = width() * fx;
		double dy = height() * fy;
		xmin_ += dx;
		xmax_ += dx;
		ymin_ += dy;
		ymax_ += dy;
	}
};

template<typename T>
ostream& operator<<(ostream& o, const window<T>& w) {
	o.precision(10);
	o << "(" << w.xmin_ << ", " << w.ymin_ << ")-(" << w.xmax_ << ", " << w.ymax_ << ")";
	return o;
}

class fract {
public:
	window<double>		def_;
	window<unsigned>	raster_ {0,0,100,50};
	double				escape_ {4.0};
	unsigned			it_ {255};
	using img_t = tuple<window<unsigned>, unsigned, vector<unsigned>>;
	unique_ptr<img_t> img_;
	using fn_t = function<unsigned(const unsigned& it, const double&, const double&)>;
	fn_t fn_;
	void compute() {
		auto i = new img_t(raster_, it_, vector<unsigned>(raster_.width() * raster_.height(), 0));
		img_.reset(i);
		double stepx = def_.width()/raster_.width();
		double stepy = def_.height()/raster_.height();
		auto& v = get<2>(*i);
		auto it = v.begin();
		
		for(unsigned y = 0; y < raster_.height(); ++y) {
			double dy = def_.ymax_ - y * stepy;
			for(unsigned x = 0; x < raster_.width(); ++x) {
				double dx = def_.xmin_ + x * stepx;
				*it = fn_(this->it_, dx, dy);
				++it;
			}
		}
		
	}
	void computeAsyn() {
		auto i = new img_t(raster_, it_, vector<unsigned>(raster_.width() * raster_.height(), 0));
		img_.reset(i);
		unsigned imgwidth = raster_.width();
		unsigned imgheight = raster_.height();
		double stepx = def_.width()/imgwidth;
		double stepy = def_.height()/imgheight;
		auto& v = get<2>(*i);
		//unsigned nrOfPixels = imgwidth * imgheight;
		unsigned hc = thread::hardware_concurrency();
		//cerr << "hc: " << hc << endl;
		//vector<thread> threads;
		vector<future<void>> futures;
		for(unsigned tnr = 0; tnr < hc; ++tnr) {
			unsigned nrOfLines = (imgheight / hc);
			unsigned starty = tnr * nrOfLines;
			unsigned stopy = starty + nrOfLines;
			if (tnr == hc - 1) {
				stopy = imgheight;
			}
			auto tf = [this, &v, starty, stopy, nrOfLines, imgwidth, imgheight, stepx, stepy]{
				auto startit = v.begin() + imgwidth * starty;
				for(unsigned y = starty; y < stopy; ++y) {
					double dy = this->def_.ymax_ - y * stepy;
					for(unsigned x = 0; x < imgwidth; ++x) {
						double dx = this->def_.xmin_ + x * stepx;
						*startit = this->fn_(this->it_, dx, dy);
						++startit;
					}
				}
				//this_thread::sleep_for(chrono::seconds(5));
			};
			//threads.push_back(move(thread(tf)));
			futures.push_back(move(async(launch::async, tf)));
			//tf();
		}
		/*
		for(auto& t : threads) {
			t.join();
		}*/
		for(auto& f: futures) {
			f.get();
		}
	}
	static const array<char, 10> pxc_;
	void printimg() {
		if (nullptr == img_)
			return;
		unsigned t = 0;
		unsigned width = get<0>(*img_).width();
		for(const auto& p : get<2>(*img_)) {
			cout << pxc_[p/static_cast<double>(it_+1)*pxc_.size()];
			if (++t % width == 0)
				cout << endl;
		}
	}
	void saveppm() {
		if (nullptr == img_)
			throw "compute image first!";
		ofstream o("bild.ppm");
		if (!o) 
			throw "error opening file";
		o << "P5" << endl;
		o << get<0>(*img_).width() << " " << get<0>(*img_).height() << endl;
		o << 256 << endl;
		//unsigned t = 0;
		//unsigned width = get<0>(*img_).width();
		double maxit = static_cast<double>(get<1>(*img_));
		for(const auto& p : get<2>(*img_)) {
			uint8_t b = p/maxit*255;
			o << b;
			/*if (++t % width == 0)
				o << endl;
			else
				o << ' ';*/
		}
	}
	template<typename T>
	static typename T::value_type itsum(const T& b, const T& e) {//} -> T::value_type { //-> decltype(*b) {
		//auto tmp = (remove_reference<decltype(*b)>::type)();
		auto tmp = typename T::value_type();
		for(auto it = b; it < e; ++it) {
			tmp += *it;
		}
		return tmp;
	}
//#define PP(x) { cout << #x ": " << (x) << endl; }
#define PP(x)
	vector<double> eqPartitionImg() {
		if (nullptr == img_)
			throw "compute image first!";
		unsigned w = get<0>(*img_).width();
		unsigned h = get<0>(*img_).height();
		//double maxit = static_cast<double>(get<1>(*img_));
		auto& v = get<2>(*img_);
		//double psum = sum(v.begin(), v.end());
		//double psum = itsum(v.begin(), v.end());
		double psum = accumulate<v.begin(), v.end());
		PP(psum);
		unsigned hc = thread::hardware_concurrency();
		PP(hc);
		double sharePerThread = psum / hc;
		PP(sharePerThread);
		vector<double> partitions(hc+1, 0.0);
		partitions[partitions.size()-1] = 1.0;
		size_t partitionNr = 0;
		double aggsum = 0.0;
		for(unsigned y = 0; y < h; ++y) {
			PP(y);
			auto beg = v.begin() + w * y;
			auto end = beg + w;
			//double partialsum = itsum(beg, end);
			double partialsum = accumulate(beg, end);
			/*
			for(; beg != end; ++beg) {
				aggsum += *beg;
			}
			*/
			PP(partialsum);
			aggsum += partialsum;
			if (aggsum >= sharePerThread) {
				partitions[++partitionNr] = y/static_cast<double>(h);
				PP(partitions[partitionNr]);
				PP(partitionNr);
				aggsum -= sharePerThread;
				if (partitionNr >= hc-1)
					break;
			}
		}
		return partitions;
	}
};
constexpr const array<char, 10> fract::pxc_ = {{
		' ', '.', ':', ';', 'i', 'l', 'I', 'N', 'M', '_'
	}};

int main(int argn, char* argv[]) {
	int ret = 0;
	try {
		fract f;
		f.def_={-1.0,-1.0,1.0,1.0};
		double fexp = 0.0;
		double fexpmax = 4.0;
		if (argn > 1) {
			fexpmax = stod(argv[1]);
		}
		if (argn > 2) {
			fexp = fexpmax;
			fexpmax = stod(argv[2]);
		}
		f.fn_ = [&fexp](const unsigned& maxit, const double& x, const double& y) {
			complex<double> cc(0,0);
			complex<double> c(x,y);
			unsigned it = 0;
			for(; it < maxit; ++it) {
				cc = pow(cc, fexp) + c;
				if (abs(cc) > 4.0)
					break;
			}
			return it;
		};
		//f.computeAsyn();
		//f.printimg();
		//f.compute();
		//f.printimg();
		
		for(; fexp <= fexpmax; fexp += 0.2) {
			f.computeAsyn();
			cout << f.def_ << " " << fexp << endl;
			f.printimg();
			for(auto p : f.eqPartitionImg()) {
				cout << p << " - ";
			}	
			//f.saveppm();
			//f.def_.move(-0.09, 0.05);
			//f.def_.zoom(0.5);
		}

		cout << endl;
		
	} catch(const exception& e) {
		cerr << "std::exception: " << e.what() << endl;
	} catch(const char* e) {
		cerr << "const char*: " << e << endl;
	} catch(...) {
		cerr << "unknown exception" << endl;
	}
	return ret;
}