#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <chrono>
#include <vector>

namespace ThreadPool {
struct ThreadPool;
struct Task {
	virtual void run() =0;
	void operator()() {
		run();
	}
};
struct StopThreadTask : public Task {
	void run() override {}
};

struct ThreadPool {
	using concurrency_t = decltype(std::thread::hardware_concurrency());
	concurrency_t n_;

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
			this->signal_stop();
			if (thread_.joinable())
				thread_.join();
		};
		void operator()() {
			std::cerr << "Thread gestartet: " << std::this_thread::get_id()  << std::endl; 
			while(keeprunning_.load() == 1) {
				std::cerr << "sleeping: " << std::this_thread::get_id()  << std::endl; 
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
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
	~ThreadPool() {
		std::cerr << __FUNCTION__ << std::endl;
		for(auto& w: workers_)
			w.signal_stop();
	}
	auto n() const noexcept { return n_; }
};
}
