// cpp1 chrono test
// clang++ --std=c++11 cpp11chrono.cpp -o cpp11chrono
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

class RValue
{
public:
	explicit RValue (arguments);	// type conversion
	RValue();									// default ctor
	RValue(const RValue&);							// copy ctor
	RValue(RValue&&);								// move ctor
	RValue& operator=(const RValue&);				// copy assignment
	RValue& operator=(RValue&&);					// move assignment
	virtual ~RValue ();

private:
	/* data */
};

int main (int argc, char const *argv[])
{
	int tmp {0};
	return tmp;
}