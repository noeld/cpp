#include <iostream>
#include <valarray>
#include <cmath>
#include <algorithm>

using namespace std;

typedef valarray<double> vt;
constexpr double pi {3.14159265358979};
void fft(vt& r, const vt& v, size_t from, size_t to) {
	//cerr << "fft from " << from << " to " << to << endl;
	auto n = (to-from);
	auto n2 = n/2;
	auto nf = from + n2;
	if (from == to - 1) {
		r[from] = v[from];
	} else {
		fft(r, v, from, nf);
		fft(r, v, nf, to);
		for (auto k = from, ne = nf; k < ne; ++k) {
			auto t = r[k];
			r[k] = t + exp(-2*pi*k/(double)n) * r[k+n2];
			r[k+n2] = t - exp(-2*pi*k/(double)n) * r[k+n2];
		}
	}
}
void fft(vt& r, const vt& v) {
	r.resize(v.size());
	fft(r, v, 0, v.size());
}

template<typename T>
void out(ostream& cout, const T& t) {
	//decltype(t.size()) n {0};
	auto ss = cout.precision(4);
	cout << t.size();
	for (decltype(t.size()) n = 0; n < min(t.size(), static_cast<decltype(t.size())>(1)); ++n) {
		cout << ": " << t[n];
	}
	for (decltype(t.size()) n = 1; n < t.size(); ++n) {
		cout << ", " << t[n];
	}
	cout << endl;
	cout.precision(ss);
}
int main (int argc, char const *argv[])
{
	//vt v{0,0.1,0.5,1.0,0.5,0.0,-0.1,-0.5,-1.0,-0.5,-0.1,0.0,0.1,0.5,1.0,0.5};
	vt v(8);
	{
		double s = 2*pi/v.size();
		double x = 0.0;
		double b = 0;
		for (auto& e : v) {
			e = sin(x) + sin(x/4.0);
			b += e*e;
			x += s;
		}
		//normieren
		auto ff = 1/sqrt(b);
		v *= ff;
	}
	vt r;
	fft(r, v);
	out(cout, v);
	out(cout, r);
	vt s;
	fft(s, r);
	out(cout, s);
	return 0;
}