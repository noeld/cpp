#include <iostream>
#include <array>

using namespace std;

class TC
{
public:
//	explicit TC (arguments);	// type conversion
	TC(){cout << __func__ << endl;}									// default ctor
	TC(const TC&) { cout << __func__ << endl ;}							// copy ctor
	TC(TC&&) { cout << __func__ << endl; }								// move ctor
	TC& operator=(const TC&);				// copy assignment
	TC& operator=(TC&&);					// move assignment
	virtual ~TC () {cout << __func__ << endl;}


	friend TC generateTC();
private:
	/* data */
	array<int,5> data_;
};

TC generateTC() {
	TC t;
	for(auto& n : t.data_) {
		n = 1;
	}
	for(const auto& n : t.data_) {
		cout << n << endl;
	}
	return t;
}

int main (int argc, char const *argv[])
{
	TC tc = generateTC();
	return 0;
}