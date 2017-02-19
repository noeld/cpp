//
// Created by Arnold Müller on 14.02.17.
//

#include "PlanetGenerator.h"

PlanetGenerator::PlanetGenerator(const DoubleRange &posRange
        , double alpha
        , double beta
        , double minMass
        , double maxMass
        , double xmax
        , double ymax
)
: massDistribution_(alpha, beta), posXDistribution_(posRange.xmin_, posRange.xmax_)
        , posYDistribution_(posRange.ymin_, posRange.ymax_)
        , rgbDistribution_(0, 255)
        , minMass_(minMass), maxMass_(maxMass)
        , xmax_(xmax), ymax_(ymax)
{

}

Planet &PlanetGenerator::Generate(Planet &p) {
    auto m = minMass_ + massDistribution_(generator_) * (maxMass_ - minMass_);
    p.setMass(m);
    auto x = posXDistribution_(generator_);
    auto y = posYDistribution_(generator_);
    p.setPos(Vector(x,y));
    int rgb = (rgbDistribution_(generator_) << 16) + (rgbDistribution_(generator_) << 8) + rgbDistribution_(generator_);
    p.setRgb_(rgb);
    p.setSpeed(Vector(0.0, 0.0));
    p.setActive(true);
    return p;
}

Planet PlanetGenerator::Generate() {
    Planet p;
    Generate(p);
    return p;
}

PlanetGenerator::~PlanetGenerator() {

}
