//
// Created by Arnold Müller on 07.04.17.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H


#include <future>
#include <vector>
#include <queue>
#include <iostream>

class Threadpool;

class Task {
public:
    enum class Status : uint8_t { Waiting, Ready, Running, Finished, Aborted };
    friend class Threadpool;

    Task() : task_number_(++counter)
             , future_(promise_.get_future())
    {
        std::cerr << __FUNCTION__ << " " << task_number_ << std::endl;
    }

    virtual ~Task() {
        std::cerr << __FUNCTION__ << " " << task_number_ << " (" << Duration().count() / 1000.0 << ")" << std::endl;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    void Operate() {
        setStatus(Status::Running);
        start_time_ = end_time_ = std::chrono::system_clock::now();
        try {
            auto r = Run();
            end_time_ = std::chrono::system_clock::now();
            setStatus(r);
            promise_.set_value(r);
        } catch(...) {
            end_time_ = std::chrono::system_clock::now();
            setStatus(Status::Aborted);
            promise_.set_exception(std::current_exception());
        }
    }
    Status getStatus() const  {
        return status_;
    }

    std::shared_future<Task::Status> getFuture() {
        return future_;
    }

    unsigned int getTask_number() const {
        return task_number_;
    }
    const std::chrono::system_clock::time_point &getStart_time() const {
        return start_time_;
    }

    const std::chrono::system_clock::time_point &getEnd_time() const {
        return end_time_;
    }
    std::chrono::milliseconds Duration() const {
        std::chrono::milliseconds d;
        switch(status_) {
            case Status::Waiting:
                break;
            case Status::Running:
                d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time_);
                break;
            case Status::Aborted:
                // fallthrough
            case Status::Finished:
                d = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_);
                break;
            case Status::Ready:
                break;
        }
        return d;
    }
protected:
    virtual Status Run() =0;
    void setStatus(Status status)  {
        status_ = status;
    }
private:

    Status status_ { Status::Ready };
    std::promise<Task::Status> promise_;
    std::shared_future<Task::Status> future_;
    static std::atomic<unsigned> counter;
    unsigned task_number_;
    mutable std::chrono::system_clock::time_point start_time_, end_time_;
};

class WaitTask : public Task {
public:
    explicit WaitTask(const std::chrono::milliseconds &duration) : duration_(duration) {
        std::cerr << __FUNCTION__ << std::endl;
    }

    virtual ~WaitTask() {
        std::cerr << __FUNCTION__ << std::endl;
    }

protected:
    Status Run() override {
        std::cout << "WaitTask " << __FUNCTION__ << " " << duration_.count() << " started" << std::endl;
        std::this_thread::sleep_for(duration_);
        std::cout << "WaitTask " << __FUNCTION__ << " " << duration_.count() << " finished" << std::endl;
        return Status::Finished;
    }

private:
    std::chrono::milliseconds duration_;
};

template<typename T>
class LambdaTask : public Task {
public:
    explicit LambdaTask(T&& lambda) : lambda_(std::move(lambda)) {
        std::cerr << __FUNCTION__ << std::endl;
    }
    virtual ~LambdaTask() {
        std::cerr << __FUNCTION__ << std::endl;
    }

protected:
    Status Run() override {
        lambda_();
        return Status::Finished;
    }

private:
    T lambda_;
};

class Threadpool {
public:
    void ThreadFunction(size_t thread_number) {
        //std::unique_lock<std::mutex> l(m_);
        for(unsigned local_task_number = 0; ; ++local_task_number) {
            auto t = DequeueNextTask(thread_number);
            if (t == nullptr) {
                std::cerr << "Thread number " << thread_number << " ending." << std::endl;
                return;
            } else {
                std::cerr << "Thread number " << thread_number << " starting task number " << t->task_number_ << std::endl;
                t->Operate();
                std::cerr << "Thread number " << thread_number << " finished task number " << t->task_number_ << std::endl;
                FinishTask(std::move(t), thread_number);
            }
        }
    }

    explicit Threadpool(unsigned n = std::thread::hardware_concurrency())
            : running_{n}
    {
        std::unique_lock<std::mutex> l(m_);
        for (unsigned i = 0; i < n; ++i) {
            threads_.emplace_back([=]{this->ThreadFunction(++counter);});
        }
    }

    virtual ~Threadpool() {
        if (active_)
            Stop();
    }

    void Push(std::shared_ptr<Task>&& task) {
        std::unique_lock<std::mutex> l(m_);
        task->setStatus(Task::Status::Waiting);
        waiting_.push(task);
        cond_push_.notify_one();
    }

    void Stop() {
        std::cerr << __FUNCTION__ << std::endl;
        active_ = false;
        cond_push_.notify_all();
        for (auto &&t : threads_) {
            t.join();
        }
    }

    void CleanupFinished() {
        std::cerr << __FUNCTION__ << std::endl;
        decltype(finished_) copy;
        {
            std::unique_lock<std::mutex> l(m_);
            copy = std::move(finished_);
            std::cerr << "finished.size() " << finished_.size() << std::endl;
        }
    }

protected:
    std::shared_ptr<Task> DequeueNextTask(size_t thread_number) {
        std::unique_lock<std::mutex> l(m_);
        while(active_ && waiting_.size() == 0) {
            cond_push_.wait(l);
        }
        if (!active_) {
            return std::shared_ptr<Task>(nullptr);
        }
        auto&& t = waiting_.front();
        running_[thread_number] = t;
        waiting_.pop();
        std::cerr << __FUNCTION__ << " " << waiting_.size() << " tasks waiting" << std::endl;
        //cond_push_.notify_one();
        return t;
    }

    void FinishTask(std::shared_ptr<Task>&& task, size_t thread_number) {
        std::unique_lock<std::mutex> l(m_);
        running_[thread_number] = std::shared_ptr<Task>(nullptr);
        finished_.push(std::move(task));
    }

private:
    std::vector<std::thread> threads_;
    std::mutex m_;
    std::queue<std::shared_ptr<Task>> waiting_;
    std::vector<std::shared_ptr<Task>> running_;
    std::queue<std::shared_ptr<Task>> finished_;
    std::condition_variable cond_push_;
    std::atomic<bool> active_ { true };
    static std::atomic<unsigned> counter;
};



#endif //THREADPOOL_THREADPOOL_H
