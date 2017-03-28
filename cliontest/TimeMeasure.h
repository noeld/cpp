//
// Created by Arnold Müller on 28.03.17.
//

#ifndef TIMEMEASURE_TIMEMEASURE_H
#define TIMEMEASURE_TIMEMEASURE_H


#include <chrono>

class TimeMeasure {
public:

    TimeMeasure(bool start = true){
        if (start) {
            Start();
        }
    }
    ~TimeMeasure() {};

    void Start() noexcept {
        init_time_point_ = std::chrono::high_resolution_clock::now();
        last_time_point_ = init_time_point_;
    }

    template<typename D = std::chrono::nanoseconds>
    D Duration() {
        auto n = std::chrono::high_resolution_clock::now();
        auto d = std::chrono::duration_cast<D>(n - last_time_point_);
        last_time_point_ = n;
        return d;
    }

    template<typename D = std::chrono::nanoseconds>
    D TotalDuration(bool til_last_timestamp = true) {
        if (til_last_timestamp) {
            return std::chrono::duration_cast<D>(last_time_point_ - init_time_point_);
        } // else
        return std::chrono::duration_cast<D>(std::chrono::high_resolution_clock::now() - init_time_point_);
    }

private:
    std::chrono::high_resolution_clock::time_point init_time_point_;
    std::chrono::high_resolution_clock::time_point last_time_point_;
};



#endif //TIMEMEASURE_TIMEMEASURE_H
