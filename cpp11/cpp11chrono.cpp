// cpp1 chrono test
// clang++ --std=c++11 cpp11chrono.cpp -o cpp11chrono
#include <iostream>
#include <chrono>
#include <vector>

using namespace std;

class TestClass
{
public:
	explicit TestClass(size_t length) // explicit type conversion
	: length_{length}, str_(new char[length+1])
	{
		cerr << "TestClass::TestClass(size_t)" << endl;
		str_[length_] = {0};
	}
	TestClass () : TestClass(0) {
		cerr << "TestClass::TestClass()" << endl;
	} // Default
	TestClass(const TestClass& rhs) // Copy Constructor
	: TestClass(rhs.length_) {
		cerr << "TestClass::TestClass(const TestClass&)" << endl;
		copy_n(rhs.str_, length_, str_);
	}
	TestClass& operator=(const TestClass&); // copy assignment
	TestClass(TestClass&& rhs) // Move ctor
	: length_{rhs.length_}, str_{rhs.str_} {
		cerr << "TestClass::TestClass(TestClass&&)" << endl;
		rhs.str_ = nullptr;
		rhs.length_ = 0;
	}
	TestClass& operator=(TestClass&&); // move assignment
	virtual ~TestClass ();
	
	size_t length() const { return length_ ;}
private:
	size_t length_ = {0};
	char* str_ = {nullptr};
};

TestClass::~TestClass() {
	cerr << "TestClass::~TestClass()" << endl;
}
TestClass& TestClass::operator=(const TestClass& rhs) {
	cerr << "TestClass::operator=(const TestClass&)" << endl;
	if (this != &rhs) {
		delete [] str_;
		length_ = rhs.length_;
		str_ = new char[length_];
		copy_n(rhs.str_, length_, str_);
	}
	return *this;
}

TestClass& TestClass::operator=(TestClass&& rhs) {
	cerr << "TestClass::operator=(TestClass&&)" << endl;
	if (this != &rhs) {
		delete [] str_;
		length_ = rhs.length_;
		str_ = rhs.str_;
		rhs.str_ = nullptr;
		rhs.length_ = 0;
	}
	return *this;
}
#define LOG(x) cerr << "*** " << #x << " ***" << endl; x

int main (int argc, char const *argv[])
{
	auto l = [](int i) {
	LOG(cout << sizeof(size_t));
	LOG(TestClass t);
	LOG(TestClass c{t});
	LOG(TestClass m{std::move(c)});
	LOG(cout << t.length() << endl);
	LOG(cout << c.length() << endl);
	LOG(TestClass& r{t});
	LOG(auto n = move(t));
	LOG(vector<TestClass> vv(i));
	for(const auto& tc : vv) {
		LOG(cout << tc.length() << endl);
	}
	//decltype(*vv.begin())
	for_each(vv.begin(), vv.end(), [](decltype(*vv.begin())& o){ cout << "[]" << o.length() << endl; });
};
	l(3);
	l(1);
	cerr << typeid(l).name() << endl;
	/* code */
	return 0;
}