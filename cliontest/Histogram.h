//
// Created by Arnold Müller on 20.02.17.
//

#ifndef CLIONTEST_HISTOGRAM_H
#define CLIONTEST_HISTOGRAM_H

#include <tuple>
#include <vector>

//template<class T, const typename T::value_type& (*accessor)(const typename T::const_iterator&)>
template<class T>
class Histogram {
public:
    using value_type = typename T::value_type;
    using float_type = float;
    using cluster = std::tuple<float_type, float_type, unsigned, float_type>;
    Histogram(unsigned resx, unsigned resy) : resx_{resx}, resy_{resy} {
        if (resx_ <= 0.0)
            resx_ = 1;
        if (resy_ <= 0.0)
            resy_ = 1;
    }

    template<class Accessor>
    void Refresh(const T* container, Accessor acc) {
        if (container != nullptr) {
            container_ = container;
        }
        if (container_ == nullptr)
            return;
        //Accessor acc;
        auto mm = std::minmax_element(container_->cbegin(), container_->cend()
                , [=](const typename T::value_type& a, const typename T::value_type& b) {return acc(a) <  acc(b); }
        );
        float_type min = acc(*mm.first);
        float_type max = acc(*mm.second);
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
            size_t index = (acc(e) - min) / clustersize_;
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
private:
    unsigned resx_, resy_;
    float_type clustersize_{0};
    float_type valuesize_ {0};
    std::vector<cluster> hist_;
    const T* container_ {nullptr};
public:
    decltype(hist_.begin()) begin() const { return hist_.begin(); }
    decltype(hist_.end()) end() const { return hist_.end(); }
    decltype(hist_.cbegin()) cbegin() const { return hist_.cbegin(); }
    decltype(hist_.cend()) cend() const { return hist_.cend(); }
};

#endif //CLIONTEST_HISTOGRAM_H
