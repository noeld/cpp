#pragma once

#include "kdtree.h"

template<typename point_type>
struct random_generator {

    explicit random_generator(size_t num_points) 
    : dist_{0, static_cast<typename point_type::value_type>(num_points)}
    {}

    std::random_device rd;
    std::uniform_int_distribution<typename point_type::value_type> dist_;

    void operator()(point_type& p) {
        for (size_t i = 0; i < point_type::size(); ++i) 
            p[i] = dist_(rd);
    }

    template<typename T>
    void for_each(T begin, T end) {
        while (begin != end) {
            operator()(*begin);
            ++begin;
        }
    }
};

