//
// Created by Arnold Müller on 14.02.17.
//

#ifndef CLIONTEST_PLANETGENERATOR_H
#define CLIONTEST_PLANETGENERATOR_H


#include <random>
#include "Range2D.h"
#include "Planet.h"

class PlanetGenerator {
public:
    using float_t = Planet::float_t;
    using vector_t = Planet::vector_t ;
    explicit PlanetGenerator(const DoubleRange& posRange
            , float_t alpha=2.1
            , float_t beta=2.0
            , float_t minMass=1
            , float_t maxMass=100
            , float_t xmax=900
            , float_t ymax=600
            , float_t minSpeed = 0.0
            , float_t maxSpeed = 10.0
    );

    virtual ~PlanetGenerator();
    PlanetGenerator(const PlanetGenerator&) = delete;
    PlanetGenerator& operator=(const PlanetGenerator&) = delete;

    Planet Generate();
    Planet& Generate(Planet&);
private:
    float_t minMass_, maxMass_, minSpeed_, maxSpeed_;
    float_t xmax_, ymax_;
    std::default_random_engine generator_ {static_cast<std::default_random_engine::result_type >(std::chrono::system_clock::now().time_since_epoch().count()) };
    std::gamma_distribution<float_t> massDistribution_;
    std::gamma_distribution<float_t> speedDistribution_ {2.0, 2.0};
    std::uniform_int_distribution<int> posXDistribution_;
    std::uniform_int_distribution<int> posYDistribution_;
    std::uniform_int_distribution<int> rgbDistribution_;
};


#endif //CLIONTEST_PLANETGENERATOR_H
