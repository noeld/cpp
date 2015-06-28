//clang++ cpp11.cpp -std=c++11 -o cpp11
#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <array>
#include <atomic>
#include <cstdint>

using namespace std;

long long func() {
	cout << std::this_thread::get_id() << endl;
	sleep(1);
	return 7ll;
}

template<typename t> struct countertype { 
	typedef t type; 
//	static const size_t bytelength{sizeof(t)}; 
};
typedef countertype<uint32_t> myct_t;

class mythread : public thread {
public:
	using thread::thread;
//	using thread::operator=;
	~mythread() {
		cout << "~mythread" << endl;
	}
};

int main (int argc, char const *argv[])
{
	cout << "Hardware concurrency: " << thread::hardware_concurrency() << endl;
	//auto var = func();
	//cout << var << endl;
	//cout << typeid(var).name() << endl;
	mutex m;
	atomic<typename myct_t::type> counter{0};
	auto lambda = [&](int max){ for(int i = 0; i < max; ++i) { 
		++counter;
		if (i % 1000 == 0)
		{ 
			{
				unique_lock<mutex> lock(m);
				cout << std::this_thread::get_id() << " " << i << "/" << max << endl; 
			}
//			sleep(1);
		}
	} };
	
	const size_t n = 32;
	array<thread*,n> t;
//	thread** t = new thread*[n];
//	for (int i = 0; i < n; ++i) {
	{
		int i{0};
		for (auto& c : t) {
			c = new thread(lambda, ++i << 12);
		}
	}
	for (auto& c : t) {
		c->join();
		delete c;
		//cout << "(" << myct_t::bytelength; 
		cout << "(" << sizeof(myct_t::type) << ") Counter: " << counter << endl;
	}
	/*
	for (int i = 0; i < n; ++i) {
		t[i]->join();
		delete t[i];
	}*/
//	delete [] t;
	
	return 0;
}