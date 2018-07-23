
#include <iostream>
#include <memory>
#include <exception>

class Mempool final {
public:
	struct MempoolExhaustedException : public std::exception {
		explicit MempoolExhaustedException(const Mempool& m, size_t requested)
		: requested_{requested}, capacity_{m.Capacity()}, used_{m.Used()}
		{}
		virtual const char* what() const noexcept override { return "Mempool exhausted"; }
		const size_t requested_, capacity_, used_;
	};
	Mempool(size_t capacity) 
	: capacity_{capacity}
	, mem_{std::make_unique<char[]>(capacity)} 
	, first_free_{mem_.get()}
	{}
	size_t Used()     const noexcept { return first_free_ - mem_.get(); }
	size_t Free()     const noexcept { return capacity_   - Used();     }
	size_t Capacity() const noexcept { return capacity_;  }
	char* Allocate(size_t requested) {
		if (requested > Free()) 
			throw MempoolExhaustedException(*this, requested);
		auto result = first_free_;
		first_free_ += requested;
		return result;
	}
private:
	size_t                  capacity_;
	std::unique_ptr<char[]> mem_;
	char*                   first_free_;
};

int main(int argc, char const *argv[])
{
	Mempool mp{100000};
	return 0;
}