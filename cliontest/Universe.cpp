//
// Created by Arnold Müller on 11.02.17.
//

#include "Universe.h"
#include "PlanetGenerator.h"

void Universe::advance(double t) {
    for(auto& p : objects_) {
        if (!p.isActive())
            continue;
        for(auto& pp : objects_) {
            if (&p == &pp)
                continue;
            if (!pp.isActive())
                continue;
            auto dir = pp.getPos() - p.getPos();
            auto dist = dir.magn();
            if (dist <= (pp.getR() + p.getR())) {
                if (&p < &pp)
                    nr_ -= Planet::Collide(p, pp);
            } else {
                double a = pp.getMass() / ( dist * dist ) * (6.6740831e-11 * 100e8);
                a *= t;
                dir.normalize() *= a;
                p.Speed() += dir;
            }
        }
    }
    for(auto& p : objects_) {
        p.advance(t);
    }
}

Universe::~Universe() {

}

Universe::Universe(size_t n) : nr_ {n} {
    objects_.resize(n);
    PlanetGenerator gen(DoubleRange(0, 0, 900, 600), 0.5, 4.0, 10000, 20000);
    for(auto& p : objects_) {
        gen.Generate(p);
    }
}

void Universe::ReadProperties(const Properties &properties) {
    nr_ = properties.generate_n;
    //properties.
}

void Universe::WriteProperties(Properties &properties) {

}
