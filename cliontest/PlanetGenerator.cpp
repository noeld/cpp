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
        , double minSpeed
        , double maxSpeed
)
: massDistribution_(alpha, beta), posXDistribution_(posRange.xmin_, posRange.xmax_)
        , posYDistribution_(posRange.ymin_, posRange.ymax_)
        , rgbDistribution_(0, 255)
        , minMass_(minMass), maxMass_(maxMass)
        , xmax_(xmax), ymax_(ymax)
        , minSpeed_ {minSpeed}, maxSpeed_{maxSpeed}
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

    auto s = minSpeed_ + speedDistribution_(generator_) * (maxSpeed_ - minSpeed_);
    auto center = Vector(xmax_ / 2.0, ymax_ / 2.0);
    Vector speed = (center - p.getPos()).orthogonal().normalize() * s;
    p.setSpeed(speed);
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
