
extern "C" {
	#include <unistd.h>
}
#include <iostream>
#include <vector>
#include <array>

class mem_t
{
public:
	explicit mem_t (size_t size) : size_{size}, mem_{new unsigned char[size]} {
	}	// type conversion
	mem_t() = delete;									// default ctor
	mem_t(const mem_t&) =delete;							// copy ctor
	mem_t(mem_t&& m) : size_{m.size_}, mem_{std::move(m.mem_)} {
		m.mem_=nullptr; 
		m.size_=0;
	}								// move ctor
	void set(unsigned char val) {
		memset(mem_.get(), val, size_);
	}
	mem_t& operator=(const mem_t&) =delete;				// copy assignment
	mem_t& operator=(mem_t&& m) { 
		size_=m.size_;
		mem_=std::move(m.mem_);
		m.size_ = 0;
		return *this; 
	}					// move assignment
	virtual ~mem_t () {
		size_=0;
	}

private:
	size_t size_;
	std::unique_ptr<unsigned char[]> mem_;
};


int main (int argc, char const *argv[])
{
	std::array<unsigned, 3> di {{5,10,15}};
	size_t ms = 1014ul*1024ul*3ul;
	std::vector<unsigned> intv(di.begin(), di.end());
	auto ii = intv.cbegin();
	do {
		std::cout << "start" << std::endl;
		mem_t m(ms);
		m.set(0);
		mem_t m2(std::move(m));
		/*std::cout << "> ";
		char b[100];
		std::cin >> b;*/
		if (ii!=intv.cend()) {
			std::cerr << "Sleeping for " << *ii << " seconds" << std::endl;
			auto s = *ii;
			++ii;
			sleep(s);
		}
	} while(ii!=intv.cend());
	return 0;
}