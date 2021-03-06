#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <chrono>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/queue.hpp>

namespace ThreadPool {
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
		c_.wait(l, [this](){ return q_.size > 0; });
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
private:
	/* data */
};
struct ThreadPool;
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
	virtual ~Task() = default;
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
struct ThreadPool {
	using concurrency_t = decltype(std::thread::hardware_concurrency());
	concurrency_t n_;
	using taskptr_t = std::unique_ptr<Task>;
	LockedQueue<taskptr_t> q_;
	LockedQueue<taskptr_t> finished_;
	class Worker;
	friend class ThreadPool::Worker;
	class Worker
	{
	public:
		explicit Worker (ThreadPool& t) : tp_{t}, thread_(std::ref(*this)) {}	// type conversion
		Worker() = delete;									// default ctor
		Worker(const Worker&) =delete;							// copy ctor
		Worker(Worker&& w) : tp_(w.tp_), thread_(std::move(w.thread_)) {
			keeprunning_ = w.keeprunning_.load();
		}	// move ctor
		Worker& operator=(const Worker&) = delete;				// copy assignment
		Worker& operator=(Worker&&) = delete;					// move assignment
		virtual ~Worker (){
			std::cerr << __FUNCTION__ << std::endl;
			if (thread_.joinable()) {
				this->signal_stop();
				thread_.join();
			}
		};
		void operator()() {
			size_t tasknum {0};
			std::cerr << "Thread gestartet: " << std::this_thread::get_id()  << std::endl; 
			taskptr_t t;
			while(keeprunning_.load() == 1) {
				std::cerr << "Waiting " << std::this_thread::get_id() << std::endl;
				tp_.q_.dequeue(t);
				++tasknum;
				std::cerr << "Got " << tasknum << " " << typeid(*t).name() << std::this_thread::get_id() << std::endl;
				(*t)();
				tp_.finished_.enqueue(std::move(t));
			}
			std::cerr << "Thread fertig: " << std::this_thread::get_id()  << std::endl; 
		}
		void signal_stop() {
			keeprunning_ = 0;
		}
		bool is_active() const noexcept {
			return keeprunning_ == 1;
		}
	private:
		ThreadPool& tp_;
		std::atomic<int> keeprunning_ {1};
		std::thread thread_;
	};

	std::vector<Worker> workers_;
	explicit ThreadPool(concurrency_t n = std::thread::hardware_concurrency()) : n_{n} {
		for(concurrency_t i = 0; i < n_; ++i) {
			workers_.emplace_back(*this);
		}
	}
	ThreadPool(const ThreadPool&) =delete;
	void stopWorkers() {
		std::cerr << __FUNCTION__ << std::endl;
		for(auto& w: workers_) {
			w.signal_stop();
			q_.enqueue(std::move(ThreadPool::taskptr_t(new StopThreadTask)));
		}
		q_.wait_empty();
		workers_.clear();
	}
	~ThreadPool() {
		std::cerr << __FUNCTION__ << std::endl;
		stopWorkers();
	}
	auto& finished() { return finished_; }
	auto& tasks() { return q_; }
	auto n() const noexcept { return n_; }
};
}
