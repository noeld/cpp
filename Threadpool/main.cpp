#include <iostream>
#include "Threadpool.h"

int main(int argn, char* argv[]) {
    unsigned n = std::thread::hardware_concurrency();
    if (argn > 1) {
        n = std::stoul(argv[1]);
    }
    Threadpool tp(n);
    Threadpool tp2(n);

    auto l = [=]{ std::this_thread::sleep_for(std::chrono::seconds(3)); };
    std::cout << sizeof(l) << std::endl;
    auto s = std::shared_ptr<Task>(new LambdaTask<decltype(l)>(std::move(l)));
    auto f = s->getFuture();
    auto l2 = [&f]{
        std::future_status st;
        do {
            std::cout << "Waiting for ready" << std::endl;
            st = f.wait_for(std::chrono::milliseconds(500));
        } while(st != std::future_status::ready);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Finished waiting for ready" << std::endl;
    };
    auto s2 = std::shared_ptr<Task>(new LambdaTask<decltype(l2)>(std::move(l2)));
    tp.Push(std::move(s));
    tp2.Push(std::move(s2));


    auto l3 = [=]{
        Threadpool tpp(4);
        unsigned nt = 0;
        for (unsigned n = 0; n < 8; ++n) {
            auto l = [=](unsigned i) {
                for(unsigned n = 0; n < i; ++i) {
                    std::cout << "Hallo " << n << "/" << i << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            };
            auto ll = std::bind(l, n*2 + 8);
            tpp.Push(std::move(std::shared_ptr<Task>(new LambdaTask<decltype(ll)>(std::move(ll)))));
            ++nt;
        }
        do {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        } while(tpp.FinishedSize() < nt);
    };
    tp.Push(std::move(std::shared_ptr<Task>(new LambdaTask<decltype(l3)>(std::move(l3)))));

    unsigned u;
    do {
        std::cin >> u;
        if (u == 99) {
            tp.CleanupFinished();
            continue;
        }
        if (u == 88) {
            auto l = [=]{ std::cout << "Lambda function " << u << std::endl; };
            tp.Push(std::move(std::shared_ptr<Task>(new LambdaTask<decltype(l)>(
                 std::move(l)
            ))));
            continue;
        }
        if (u > 0)
            tp.Push(std::move(std::shared_ptr<Task>(new WaitTask(std::chrono::seconds(u)))));
    } while( u > 0 );
    return 0;
}