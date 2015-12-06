#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

using nv_t = vector<int>;

istream& operator>>(istream& in, nv_t& v) {
	nv_t::value_type val;
	while(in.good()) {
		in >> val;
		v.push_back(val);
	}
	return in;
}

struct SubArrayDef {
	size_t start {0};
	size_t end {0};
	nv_t::value_type sum {0};
};

ostream& operator<<(ostream& o, const SubArrayDef& sad) {
	o << "From: " << sad.start << "; To: " << sad.end << "; Sum: " << sad.sum;
	return o;
}

SubArrayDef FindGreatestSubArray(const nv_t& v) {
	SubArrayDef sad;
	sad.start = 0;
	sad.end = 0;
	if (v.size() > 0) {
		sad.sum = v[0];
		auto s = sad.start;
		auto e = sad.start;
		auto sum = 0;
		size_t it = 0;
		while(s < v.size()) {
			++it;
			sum += v[e];
			if (sum > sad.sum) {
				sad.start = s; sad.end = e; sad.sum = sum;
				//cout << sad << endl;
			}
			if (sum < 0) {
				sum = 0;
				s = e + 1;
				e = s;
			}/*
			if (++e > v.size()) {
				++s;
				e = s;
				sum = 0;
			}*/
		}
		//cout << "s = " << s << " It = " << it << endl;
	}
	return sad;
}

int main (int argc, char const *argv[])
{
	int ret = 0;
	constexpr const char * const filename = "numbers.txt";
	constexpr size_t expected_size = 1000;
	nv_t nv;
	nv.reserve(expected_size);
	try {
		ifstream in(filename);
		if (in.good()) {
			in >> nv;
			cerr << nv.size() << " elements read." << endl;
			for(auto n : nv) {
				if (n < 0) { cout << n << " "; }
			}
			cout << endl;
			auto sad = FindGreatestSubArray(nv);
			cout << sad << endl;
		} else {
			cerr << "Could not open " << filename << endl;
		}
	} catch(...) {
		cerr << "Unknown exception" << endl;
	}
	return ret;
}