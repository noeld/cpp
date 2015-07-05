#include <iostream>
#include <vector>
#include <unistd.h>

template<typename T>
decltype(auto) get(const T& v, size_t i) {
	return v[i];
}

template<typename T> class TC;

int main (int argc, char const *argv[])
{
	using namespace std;
	vector<int> v{1, 2, 3, 55, 3, 2};
	auto e = get(v, 4);
//	TC<decltype(e)> test;
	cout << get(v, 3) << endl;
	/* code */
	return 0;
}