//# PATH=/usr/local/gcc-5.3.0/bin:$PATH gcc-5.3.0 textgcc.cpp -std=c++14 -o textgcc -L/usr/local/gcc-5.3.0/lib -lstdc++ -O2 -Wall 

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <regex>
#include <memory>

using namespace std;

auto test(int i) -> float  {
	return i-1;
}

int main (int argc, char const *argv[])
{
	int ret = 0;
	vector<string> params(argv, argv + argc);
	auto l = []{cout << "Thread" <<  4 + 5 << endl;};
	auto t = thread(l);
	auto tt = thread([&params]{ size_t n = 0; for(const auto& p : params){ cout << ++n << ": " << p << endl; }});
	t.join();
	tt.join();
	regex rr("^[+-]?[0-9]+(?:\\.[0-9]*)?$");
	size_t n = 0;
	for(const auto& p : params) {
		cout << ++n << ": " << p;
		if (regex_match(p, rr)) {
			cout << " float";
		}
		cout << endl;
	}
	{
		size_t mem = 4ull << 30;
		cout << mem/1024/1024 << endl;
		auto m = unique_ptr<char[]>(new char[mem]);
		memset(m.get(), 0, mem);
	//	delete [] m;
		char c;
		cin >> noskipws >> c;
	}
	char c;
	cin >> noskipws >> c;
	return ret;
}