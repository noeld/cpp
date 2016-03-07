//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib -o apfel2 apfel2.cpp -lboost_system -lboost_filesystem -lboost_iostreams -lcryptopp

#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <complex>
#include <boost/format.hpp>
#include <utility>

using namespace std;

template<typename T>
struct window {
	window(const T& xmin, const T& ymin, const T& xmax, const T& ymax) 
		: xmin_{xmin}, ymin_{ymin}, xmax_{xmax}, ymax_{ymax}
	{
		if (xmin_ > xmax_) 
			swap(xmin_, xmax_);
		if (ymin_ > ymax_) 
			swap(ymin_, ymax_);
	}
	window() = default;
	T width() const { return xmax_ - xmin_; }
	T height() const { return ymax_ - ymin_; }
	void zoom(const double& fx, const double& fy) {
		//double ffx = 1.0/fx;
		//double ffy = 1.0/fy;
		double dx = width() * fx/2.0;
		double dy = height() * fy/2.0;
		xmin_ += dx;
		xmax_ -= dx;
		ymin_ += dy;
		ymax_ -= dy;
	}
	void zoom(const double& f) {
		zoom(f, f);
/*
		double ff = 1.0/f;
		double dx = width() * ff/2.0;
		double dy = height() * ff/2.0;
		xmin_ += dx;
		xmax_ -= dx;
		ymin_ += dy;
		ymax_ -= dy;*/
	}
	void move(const double& h, const double& v) {
		double dx = width() * h;
		double dy = height() * v;
		xmin_ += dx;
		xmax_ += dx;
		ymin_ += dy;
		ymax_ += dy;
	}
	T xmin_ {-2};
	T ymin_ {-1.5};
	T xmax_ {1};
	T ymax_ {1.5};
};

struct fract {
	using it_t = unsigned;
	window<double>     imgrange_;
	window<unsigned>   dim_;
	
};

template<typename T>
ostream& operator<<(ostream& o, const window<T>& w) {
	o << boost::format{"(%1.15f, %1.15f)-(%1.15f, %1.15f)"} % w.xmin_ % w.ymin_ % w.xmax_ % w.ymax_;
	return o;
}

constexpr const array<char, 13> ch = {{ ' ', '.', ',', ':', ';', '+', 'i', 'I', '%', 'H', '8', 'M', '#'}};

int main (int argc, char const *argv[])
{
	using cpt = complex<double>;
	double escval = 4.0;
	vector<string> params(argv, argv + argc);
//	cin >> noskipws;
	try {
//		window<unsigned> p(0, 100, 0, 20);
		cpt ca(1,0.5);
/*		cout << ca << endl;
		cout << ca * ca * ca << endl;
		cout << pow(ca,3) << endl;*/
		window<double> w;//(-1.5,0,0.5,1);
		unsigned maxit = 100;
		unsigned rows = 120;
		unsigned lines = rows * 1.0/1.8;
		for(unsigned it = 0; true; ++it) {
			if (it > 0) {
				cout << w << " " << maxit << " " << escval << " zoom In, zoom Out, Left, Right, Up, Down, +iterate, -iterate, +Esc, -esc, +Wide, -wide, Quit: ";
				string cc;
				getline(cin, cc);
				char c;
				c = cc.at(0);
				cout << endl;
				switch(c) {
					case 'q' :
						exit(0);
						break;
					case 'l':
						w.move(-0.25, 0);
						break;
					case 'r':
						w.move(0.25, 0);
						break;
					case 'u':
						w.move(0,0.25);
						break;
					case 'd':
						w.move(0,-0.25);
						break;
					case 'i':
						w.zoom(0.5);
						break;
					case 'o':
						w.zoom(-1);
						break;
					case '+':
						maxit *= 2.0;
						maxit = min(1u<<16,maxit);
						break;
					case '-':
						maxit /= 2.0;
						maxit = max(1u,maxit);
						break;
					case 'e':
						escval /= 2.0;
						escval = max(1.0, escval);
						break;
					case 'E':
						escval *= 2.0;
						escval = min(256.0, escval);
						break;
					case 'W':
						rows += rows * 0.25;
						w.zoom(-0.25, 0);
						break;
					case 'w':
						rows -= rows/4.0;
						w.zoom(0.25, 0);
						break;
				}
			}
			double stepx = w.width() / static_cast<double>(rows);
			double stepy = w.height() / static_cast<double>(lines);
			cout << stepx << ", " << stepy << endl;
			//cout << endl;
			for(double y = w.ymax_; y > w.ymin_; y -= stepy) {
				for(double x = w.xmin_; x < w.xmax_; x += stepx) {
					unsigned i = 0;
					bool isin = true;
					cpt c(x,y);
					cpt cc = c;
					for(; i < maxit; ++i) {
						c = c * c *c + cc;
						if (c.real() * c.real() + c.imag() * c.imag() > escval) {
							isin = false;
							break;
						} 
					}
					if (isin) {
						cout << '*';
					} else {
						size_t idx = static_cast<double>(i)/maxit * ch.size();
						cout << ch[idx] ;
					}
				}
				cout << endl;
			}
		}
	} catch(...) {
		cerr << "Unknown exception" << endl;
	}
	return 0;
}