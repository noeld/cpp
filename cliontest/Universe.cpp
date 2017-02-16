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
            if (&p < &pp && dist < pp.getR() + p.getR()) {
                Planet::Collide(p, pp);
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

Universe::Universe(size_t n) {
    objects_.resize(n);
    PlanetGenerator gen(DoubleRange(0, 0, 400, 400), 0.1, 9.0, 50, 1000);
    for(auto& p : objects_) {
        gen.Generate(p);
    }
}
