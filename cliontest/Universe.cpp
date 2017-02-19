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
                    nr_ -= Planet::Collide(p, pp, this->universe_collision_k, this->universe_collision_joindist);
            } else {
                double a = pp.getMass() / ( dist * dist ) * this->universe_g;
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

}

void Universe::Initialize() {
    objects_.resize(generate_n);
    PlanetGenerator gen(
            DoubleRange(0, 0, 900, 600)
            , generate_mass_alpha, generate_mass_beta
            , generate_mass_min, generate_mass_max
    );
    for(auto& p : objects_) {
        gen.Generate(p);
    }
}

void Universe::ReadProperties(const Properties &properties) {
    universe_g = properties.universe_g;
    universe_collision_joindist = properties.universe_collision_joindist;
    universe_collision_k = properties.universe_collision_k;
    generate_n = properties.generate_n;
    generate_speed_max = properties.generate_speed_max;
    generate_border_part = properties.generate_border_part;
    generate_mass_min = properties.generate_mass_min;
    generate_mass_max = properties.generate_mass_max;
    generate_mass_alpha = properties.generate_mass_alpha;
    generate_mass_beta = properties.generate_mass_beta;
}

void Universe::WriteProperties(Properties &properties) {
    properties.universe_g = this->universe_g;
    properties.universe_collision_joindist = this->universe_collision_joindist;
    properties.universe_collision_k = this->universe_collision_k;
    properties.generate_n = this->generate_n;
    properties.generate_speed_max = this->generate_speed_max;
    properties.generate_border_part = this->generate_border_part;
    properties.generate_mass_min = this->generate_mass_min;
    properties.generate_mass_max = this->generate_mass_max;
    properties.generate_mass_alpha = this->generate_mass_alpha;
    properties.generate_mass_beta = this->generate_mass_beta;
}
