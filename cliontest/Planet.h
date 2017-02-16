//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_PLANET_H
#define CLIONTEST_PLANET_H


#include "Vector.h"

class Planet {
public:
    Planet();
    Planet(const Vector &pos_, const Vector &speed_, double mass_, bool active);
    Planet(const Planet&) = default;
    Planet(Planet&&) = default;
    Planet& operator=(const Planet&) = default;
    ~Planet();

    bool operator==(const Planet &rhs) const {
        return std::tie(pos_, speed_, mass_, r_, active) ==
               std::tie(rhs.pos_, rhs.speed_, rhs.mass_, rhs.r_, rhs.active);
    }

    bool operator!=(const Planet &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Planet &rhs) const {
        return std::tie(pos_, speed_, mass_, r_, active) <
               std::tie(rhs.pos_, rhs.speed_, rhs.mass_, rhs.r_, rhs.active);
    }

    bool operator>(const Planet &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Planet &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Planet &rhs) const {
        return !(*this < rhs);
    }

    Planet& advance(double t) {
        this->pos_ += this->speed_ * t;
        return *this;
    }

    static double RadiusFromMass(const double& m);

    static void Join(Planet &a, Planet &b);
    static void Collide(Planet& a, Planet& b, const double& k = 0.05, const double& d = 1.0);

    const Vector &getPos() const {
        return pos_;
    }

    void setPos(const Vector &pos_) {
        Planet::pos_ = pos_;
    }

    Vector &Speed() {
        return speed_;
    }

    const Vector &getSpeed() const {
        return speed_;
    }

    void setSpeed(const Vector &speed_) {
        Planet::speed_ = speed_;
    }

    double getMass() const {
        return mass_;
    }

    void setMass(double mass_) {
        Planet::mass_ = mass_;
        r_ = Planet::RadiusFromMass(mass_);
    }

    double getR() const {
        return r_;
    }

    void setR(double r_) {
        Planet::r_ = r_;
    }

    bool isActive() const {
        return active;
    }

    void setActive(bool active) {
        Planet::active = active;
    }

private:
    Vector pos_;
    Vector speed_;
    double mass_ {1.0};
    double r_;
    bool active {true};
};


#endif //CLIONTEST_PLANET_H
