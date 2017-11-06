#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
	unordered_map<size_t, size_t> m;
	int i = 5;
	const volatile int* j = &i;
	cout << "Test" << *j << endl;
	return 0;
}