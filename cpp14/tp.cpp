#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <atomic>

template<typename T>
class LockedQueue
{
	std::mutex m_;
	std::condition_variable c_;
	std::queue<T> q_;
public:
//	explicit LockedQueue (arguments);	// type conversion
	LockedQueue() {};									// default ctor
	LockedQueue(const LockedQueue&) =delete;							// copy ctor
	LockedQueue(LockedQueue&&) =delete;								// move ctor
	LockedQueue& operator=(const LockedQueue&) =delete;				// copy assignment
	LockedQueue& operator=(LockedQueue&&) =delete;					// move assignment
	~LockedQueue (){};
	void enqueue(const T& e) {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		q_.push(e);
		c_.notify_one();
	}
	void enqueue(T&& e) {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		q_.push(std::move(e));
		c_.notify_one();
	}
	void dequeue(T& e) {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		c_.wait(l, [this](){ return q_.size() > 0; });
		e = std::move(q_.front());
		q_.pop();
	}
	void dequeue(T&& e) {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		c_.wait(l, [this](){ return q_.size() > 0; });
		e = std::move(q_.front());
		q_.pop();
	}
	bool try_dequeue(T& e) {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		if (!q_.empty()) {
			e = std::move(q_.front());
			return true;
		}
		return false;
	}
	void wait_empty() {
		std::unique_lock<std::mutex> l(m_);
		std::cerr << __FUNCTION__ << std::endl;
		//c_.notify_all();
		c_.wait(l, [this]{ return q_.empty(); });
	}
};

class Task {
public:
	using time_t = std::chrono::high_resolution_clock::time_point;
	using duration_t = std::chrono::high_resolution_clock::duration;
private:
	time_t created_at_{std::chrono::high_resolution_clock::now()};
	time_t startet_at_;
	time_t finished_at_;
public:
	auto created_at() const noexcept { return created_at_; }
	auto startet_at() const noexcept { return startet_at_; }
	auto finished_at() const noexcept { return finished_at_ ; }
	duration_t duration_net() const noexcept { return finished_at_ > created_at_ ? (finished_at_ - created_at_) : duration_t{0}; }
	duration_t duration() const noexcept { return finished_at_ > startet_at_ ? (finished_at_ - startet_at_) : duration_t{0}; }
	virtual void run() =0;
	void operator()() {
		startet_at_ = std::chrono::high_resolution_clock::now();
		run();
		finished_at_ = std::chrono::high_resolution_clock::now();
	}
	virtual ~Task() {
		std::cerr << "Task duration: " << duration().count() << std::endl;
		std::cerr << "Task duration_net: " << duration_net().count() << std::endl;
	};
};
struct SystemTask : public Task {
};
struct StopThreadTask : public SystemTask {
	void run() override { std::cerr << __FUNCTION__ << std::endl; }
};
template<typename T>
struct LambdaTask : public Task {
	T f_;
	LambdaTask(T& f) : f_{f_}{}
	LambdaTask(T&& f) : f_(std::move(f)) {}
	virtual void run() override {
		f_();
	}
};

class ThreadPool
{
	LockedQueue<Task*> waiting_;
	LockedQueue<Task*> finished_;
public:
	explicit ThreadPool (unsigned n = std::thread::hardware_concurrency()) {
		auto f = [this]{
			std::cerr << "Thread started " << std::this_thread::get_id() << std::endl;
			while(true) {
				Task* e = nullptr;
				waiting_.dequeue(e);
				if (e == nullptr) 
					break;
				(*e)();
				finished_.enqueue(std::move(e));
			}
			std::cerr << "Thread finished " << std::this_thread::get_id() << std::endl;
		};
		for(unsigned i = 0; i<n; ++i) {
			t_.emplace_back(f);
		}
	}	// type conversion
	//ThreadPool() =delete;									// default ctor
	ThreadPool(const ThreadPool&) =delete;							// copy ctor
	ThreadPool(ThreadPool&& t) : t_{std::move(t.t_)} {}								// move ctor
	ThreadPool& operator=(const ThreadPool&) =delete;				// copy assignment
	ThreadPool& operator=(ThreadPool&& t) {
		if (&t != this) {
			t_ = std::move(t.t_);
		}
		return *this;
	};					// move assignment
	virtual ~ThreadPool () {
		for(auto& t : t_) {
			if (t.joinable())
				t.join();
		}
	}
	void enqueue(Task* t) {
		waiting_.enqueue(t);
	}
	void dequeue(Task*&& t) {
		finished_.dequeue(std::move(t));
	}
private:
	std::vector<std::thread> t_;
};

int main (int argc, char const *argv[])
{
	int ret {0};
	ThreadPool tp;
	Task* t1 = new LambdaTask<std::function<void(void)>>([]{ std::cout << "Das ist der erste Task" << std::endl; std::this_thread::sleep_for(std::chrono::milliseconds(3000)); });
	Task* t2 = new LambdaTask<std::function<void(void)>>([]{ std::cout << "Das ist der andere Task" << std::endl; });
	tp.enqueue(t1);
	tp.enqueue(t2);
	
	delete t1;
	delete t2;
	return ret;
}