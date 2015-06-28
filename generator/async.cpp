#include <iostream>
#include <future>
#include <valarray>
#include <cstdint>
extern "C" { 
	#include <unistd.h>
}
#include <string>
using namespace std;
typedef valarray<uint32_t> vt;

vt::value_type fff(vt& v) {
	for(auto& e : v) {
		e = 0;
	}
	return v.sum();
}

int main (int argc, char const *argv[])
{
	constexpr size_t num { 30 << 20 };
	
	vt v(num);
	auto f = [](vt* v) -> vt::value_type { 
//		usleep(1000);
		for(auto& e : *v) {
			e = 1;
		}
		return v->sum();
	};
	auto fu = std::async(f,&v);
//	cout << v.sum() << endl;
	cout << fu.get() << endl;
//	cout << v.sum() << endl;
	string s;
	getline(cin, s);
	/* code */
	return 0;
}