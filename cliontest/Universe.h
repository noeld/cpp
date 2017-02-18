//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_UNIVERSE_H
#define CLIONTEST_UNIVERSE_H


#include <vector>
#include <chrono>
#include "Planet.h"

class Universe {
public:
    explicit Universe(size_t);

    virtual ~Universe();

    using time_point = std::chrono::high_resolution_clock::time_point;
    //using now = std::chrono::high_resolution_clock::now;

    const std::vector<Planet> &getObjects() const {
        return objects_;
    }
    void advance(double t);

    size_t getNr_() const { return nr_; }
private:
    std::vector<Planet> objects_;
    time_point last_time_;
    size_t nr_;
};


#endif //CLIONTEST_UNIVERSE_H
