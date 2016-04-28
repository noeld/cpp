// calculate mandelbrot 

#include <iostream>
#include <complex>
#include <numeric>
#include <array>
#include <string>

using namespace std;

using float_tp = double;

int main(int argc, char const *argv[])
{
	//const array<char, 4> chars {' ', '.', '|', '_'};
	//const array<char, 12> chars {' ', '.', ':', ';', '+', '=', 'o', 'O', '8', 'M', '#', '_'};
	const array<wchar_t, 12> chars {' ', '.', ':', ';', '+', '=', 'o', 'O', '8', 'M', '#', '_'};
	constexpr const float_tp tp[] = {-0.7, 0.6, -0.5, 0.7};
	constexpr const float_tp xmin = static_cast<float_tp>(tp[0]);
	constexpr const float_tp ymin = static_cast<float_tp>(tp[1]);
	constexpr const float_tp xmax = static_cast<float_tp>(tp[2]);
	constexpr const float_tp ymax = static_cast<float_tp>(tp[3]);
	
	unsigned maxit = 1000;
	if (argc > 1) {
		maxit = stoul(argv[1]);
	}

	constexpr const unsigned cols = 260;
	constexpr const unsigned rows = 80;

	auto mandel = [=](const float_tp& x, const float_tp& y) -> unsigned {
		complex<float_tp> c(x,y);
		complex<float_tp> cc(c);
		for(unsigned i = 0; i < maxit; ++i) {
			cc = pow(cc, static_cast<float_tp>(2.0)) + c;
			if (abs(cc) > static_cast<float_tp>(4.0))
				return i;
		}
		return maxit-1;
	};

	//string s;
	wstring s;
	for(float_tp y = ymax; y > ymin; y-=(ymax-ymin)/static_cast<float_tp>(rows)) {
		for(float_tp x = xmin; x < xmax; x+=(xmax-xmin)/static_cast<float_tp>(cols)) {
			unsigned i = mandel(x, y);
			s.push_back(chars[static_cast<unsigned>(i/static_cast<float_tp>(maxit) * chars.size())]);
		}
		wcout << s << "\n";
		s.clear();
	}


	return 0;
}