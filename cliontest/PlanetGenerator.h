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
    explicit PlanetGenerator(const DoubleRange& posRange, double alpha=0.1, double beta=9.0, double minMass=1, double maxMass=100);

    virtual ~PlanetGenerator();
    PlanetGenerator(const PlanetGenerator&) = delete;
    PlanetGenerator& operator=(const PlanetGenerator&) = delete;

    Planet Generate();
    Planet& Generate(Planet&);
private:
    double minMass_, maxMass_;
    std::default_random_engine generator_ {static_cast<std::default_random_engine::result_type >(std::chrono::system_clock::now().time_since_epoch().count()) };
    std::gamma_distribution<double> massDistribution_;
    std::uniform_int_distribution<int> posXDistribution_;
    std::uniform_int_distribution<int> posYDistribution_;
};


#endif //CLIONTEST_PLANETGENERATOR_H
