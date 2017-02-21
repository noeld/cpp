//
// Created by Arnold Müller on 20.02.17.
//

#ifndef CLIONTEST_HISTOGRAM_H
#define CLIONTEST_HISTOGRAM_H

#include <tuple>
#include <vector>

template<class T>
class Histogram {
public:
    using value_type = T::value_type;
    using float_type = float;
    using cluster = std::tuple<float_type, float_type, unsigned, float_type>;
    Histogram(float_type resx, float_type resy) : resx_{resx}, resy_{resy} {
        if (resx_ <= 0.0)
            resx_ = 1;
        if (resy_ <= 0.0)
            resy_ = 1;
    }

    void Refresh(const T* container = nullptr) {
        if (container != nullptr) {
            container_ = container;
        }
        if (container_ == nullptr)
            return;
        auto mm = std::minmax_element(container_->cbegin(), container_->cend());
        float_type min = *mm.first;
        float_type max = *mm.second;
        clustersize_ = (max - min) / resx_;
        hist_.resize(resx_);
        float_type f = min;
        for(auto& e : hist_) {
            std::get<0>(e) = f;
            f += clustersize_;
            std::get<1>(e) = f;
            std::get<2>(e) = 0;
            std::get<3>(e) = 0.0;
        }
        for(const auto& e : *container_) {
            size_t index = (e - min) / clustersize_;
            ++std::get<2>(hist_[index]);
        }
        unsigned hmax = std::get<2>(*std::max_element(hist_.cbegin(), hist_.cend()
                , [=](const cluster& a, const cluster& b) -> bool { return std::get<2>(a) < std::get<2>(b); }
                ));
        if (hmax == 0)
            hmax = 1;
        valuesize_ = hmax / resy_;
        float_type ff = resy_ / hmax;
        for(auto& e : hist_) {
            std::get<3>(e) = std::get<2>(e) * ff;
        }
    }
    decltype(hist_.begin()) begin() { return hist_.begin(); }
    decltype(hist_.end()) end() { return hist_.end(); }
private:
    float_type resx_, resy_;
    float_type clustersize_{0};
    float_type valuesize_ {0};
    std::vector<cluster> hist_;
    const T* container_ {nullptr};
};

#endif //CLIONTEST_HISTOGRAM_H
