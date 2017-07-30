//
// Created by Arnold Müller on 11.02.17.
//

#include "Universe.h"
#include "PlanetGenerator.h"

void Universe::advance(Planet::float_t t) {
    //for(auto& p : objects_) {
    for(size_t i = 0; i < activeObjects; ++i) {
        //for(auto& pp : objects_) {
        //if (!objects_[i].isActive())
          //  continue;
        for(size_t j = 0; j < activeObjects; ++j) {
            if (i == j)
                continue;
            //if (!objects_[j].isActive())
              //  continue;
            auto dir = objects_[j].getPos() - objects_[i].getPos();
            auto dist = dir.length();
            if (dist <= (objects_[j].getR() + objects_[i].getR())) {
                if (i < j) {
                    Planet::Collide(objects_[i], objects_[j], this->universe_collision_k,
                                    this->universe_collision_joindist);
                    if (!objects_[i].isActive()) {
                        --activeObjects;
                        std::swap(objects_[i], objects_[activeObjects]);
                        //break;
                    } else if (!objects_[j].isActive()) {
                        --activeObjects;
                        std::swap(objects_[j], objects_[activeObjects]);
                    }
                }
            } else {
                Planet::float_t a = objects_[j].getMass() / ( dist * dist ) * this->universe_g;
                a *= t;
                dir.normalize() *= a;
                objects_[i].Speed() += dir;
            }
        }
        const Planet::float_t ddd = -200;
        auto& pos = objects_[i].getPos();
        auto& speed = objects_[i].getSpeed();
        Planet::vector_t v;
        if (pos.getX() < ddd && speed.getX() < 0) {
            v.setX(t * universe_g * -speed.getX());
        }
        if (pos.getX() > generate_pos_xmax - ddd && speed.getX() > 0) {
            v.setX(t * universe_g * -speed.getX());
        }
        if (pos.getY() < ddd && speed.getY() < 0) {
            v.setY(t * universe_g * -speed.getY());
        }
        if (pos.getY() > generate_pos_ymax - ddd && speed.getY() > 0) {
            v.setY(t * universe_g * -speed.getY());
        }
        objects_[i].Speed() += v * 10;
    }
    //for(auto& p : objects_) {
    for(size_t i = 0; i < activeObjects; ++i) {
        objects_[i].advance(t);
    }
    nr_ = activeObjects;
}

Universe::~Universe() {

}

Universe::Universe(size_t n) : nr_ {n} {

}

void Universe::Initialize() {
    objects_.resize(generate_n);
    PlanetGenerator gen(
            DoubleRange(0.0, 0.0, generate_pos_xmax, generate_pos_ymax)
            , generate_mass_alpha, generate_mass_beta
            , generate_mass_min, generate_mass_max
            , generate_pos_xmax, generate_pos_ymax
            , generate_speed_min, generate_speed_max
    );
    for(auto& p : objects_) {
        gen.Generate(p);
    }
    this->activeObjects = generate_n;
}

void Universe::ReadProperties(const Properties &properties) {
    universe_g = properties.universe_g;
    universe_collision_joindist = properties.universe_collision_joindist;
    universe_collision_k = properties.universe_collision_k;
    generate_n = properties.generate_n;
    generate_speed_min = properties.generate_speed_min;
    generate_speed_max = properties.generate_speed_max;
    generate_border_part = properties.generate_border_part;
    generate_mass_min = properties.generate_mass_min;
    generate_mass_max = properties.generate_mass_max;
    generate_mass_alpha = properties.generate_mass_alpha;
    generate_mass_beta = properties.generate_mass_beta;
    generate_pos_xmax = properties.generate_pos_xmax;
    generate_pos_ymax = properties.generate_pos_ymax;

}

void Universe::WriteProperties(Properties &properties) {
    properties.universe_g = this->universe_g;
    properties.universe_collision_joindist = this->universe_collision_joindist;
    properties.universe_collision_k = this->universe_collision_k;
    properties.generate_n = this->generate_n;
    properties.generate_speed_min = this->generate_speed_min;
    properties.generate_speed_max = this->generate_speed_max;
    properties.generate_border_part = this->generate_border_part;
    properties.generate_mass_min = this->generate_mass_min;
    properties.generate_mass_max = this->generate_mass_max;
    properties.generate_mass_alpha = this->generate_mass_alpha;
    properties.generate_mass_beta = this->generate_mass_beta;
    //properties.generate_pos_xmax = this->generate_pos_xmax; // set by Frame
    //properties.generate_pos_ymax = this->generate_pos_ymax; // set by Frame
}
