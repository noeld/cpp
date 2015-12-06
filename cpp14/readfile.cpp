//clang++ -std=c++1y -Wall readfile.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main (int argc, char const *argv[])
{
	vector<string> params(argv, argv+argc);
	for(const auto& a : params) {
		cout << a << endl;
	}
	return 0;
}