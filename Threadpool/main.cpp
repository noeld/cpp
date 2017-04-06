#include <iostream>
#include <future>
#include <vector>
#include <queue>

class Threadpool;

class Task {
public:
    enum class Status : uint8_t { Waiting, Ready, Running, Finished, Aborted };
    friend class Threadpool;

    void Operate() {
        setStatus(Status::Running);
        auto r = Run();
        setStatus(r);
    }
    Status getStatus() const  {
        return status_;
    }

protected:
    virtual Status Run() =0;
    void setStatus(Status status)  {
        status_ = status;
    }
private:
    Status status_ { Status::Ready };
    std::shared_future<Task::Status> future_;
};

class WaitTask : public Task {
public:
    explicit WaitTask(const std::chrono::milliseconds &duration) : duration_(duration) {}

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

class Threadpool {
public:
    void ThreadFunction(size_t thread_number) {
        //std::unique_lock<std::mutex> l(m_);
        for(unsigned task_number = 0; ; ++task_number) {
            auto t = StartTask(thread_number);
            if (t == nullptr) {
                std::cerr << "Thread number " << thread_number << " ending." << std::endl;
                return;
            } else {
                std::cerr << "Thread number " << thread_number << " starting task number " << task_number << std::endl;
                t->Run();
                FinishTask(std::move(t), thread_number);
            }
        }
    }

    explicit Threadpool(unsigned n = std::thread::hardware_concurrency())
    : running_{n}
    {
        std::unique_lock<std::mutex> l(m_);
        for (unsigned i = 0; i < n; ++i) {
            threads_.emplace_back([=]{this->ThreadFunction(i);});
        }
    }

    virtual ~Threadpool() {
        if (active_)
            Stop();
    }

    void Push(std::shared_ptr<Task>&& task) {
        std::unique_lock<std::mutex> l(m_);
        waiting_.push(task);
        task->setStatus(Task::Status::Waiting);
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
        std::unique_ptr<decltype(finished_)> ptr;
        {
            std::unique_lock<std::mutex> l(m_);
            decltype(finished_) copy(std::move(finished_));
            std::cerr << "finished.size() " << finished_.size() << std::endl;
        }
        /*for(auto i = ptr->size(); i > 0; --i) {
            ptr->pop();
        }*/
    }

protected:
    std::shared_ptr<Task> StartTask(size_t thread_number) {
        std::unique_lock<std::mutex> l(m_);
        while(waiting_.size() == 0) {
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
};


int main(int argn, char* argv[]) {
    unsigned n = std::thread::hardware_concurrency();
    if (argn > 1) {
        n = std::stoul(argv[1]);
    }
    Threadpool tp(n);
    unsigned u;
    do {
        std::cin >> u;
        if (u == 99) {
            tp.CleanupFinished();
            continue;
        }
        tp.Push(std::move(std::shared_ptr<Task>(new WaitTask(std::chrono::seconds(u)))));
    } while( u > 0 );
    return 0;
}