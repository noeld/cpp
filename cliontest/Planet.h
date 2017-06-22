//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_PLANET_H
#define CLIONTEST_PLANET_H


#include "Vector.h"
#include "Properties.h"

class PlanetPropertyReaderWriter;

class Planet {
public:
    using float_t = float;
    using vector_t = Vector<float_t>;
    Planet();
    Planet(const vector_t &pos_, const vector_t &speed_, float_t mass_, bool active);
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

    Planet& advance(float_t t) {
        this->pos_ += this->speed_ * t;
        return *this;
    }

    static float_t RadiusFromMass(const float_t& m);
    static unsigned BlendWeightedRGB(unsigned, float_t, unsigned, float_t);
    static void Join(Planet &a, Planet &b);
    static unsigned Collide(Planet& a, Planet& b, const float_t& k = 0.05, const float_t& d = 2.2);
    const vector_t &getPos() const {

        return pos_;
    }

    void setPos(const vector_t &pos_) {
        Planet::pos_ = pos_;
    }

    vector_t &Speed() {
        return speed_;
    }

    const vector_t &getSpeed() const {
        return speed_;
    }

    void setSpeed(const vector_t &speed_) {
        Planet::speed_ = speed_;
    }

    float_t getMass() const {
        return mass_;
    }

    void setMass(float_t mass_) {
        Planet::mass_ = mass_;
        r_ = Planet::RadiusFromMass(mass_);
    }

    float_t getR() const {
        return r_;
    }

    void setR(float_t r_) {
        Planet::r_ = r_;
    }

    bool isActive() const {
        return active;
    }

    void setActive(bool active) {
        Planet::active = active;
    }

    unsigned int getRgb_() const {
        return rgb_;
    }

    void setRgb_(unsigned int rgb_) {
        Planet::rgb_ = rgb_;
    }
private:
    vector_t pos_;
    vector_t speed_;
    float_t mass_ {1.0};
    float_t r_;
    unsigned rgb_{0};
    static float_t universe_planet_density;
    bool active {true};

    friend class PlanetPropertyReaderWriter;
};

class PlanetPropertyReaderWriter : public PropertiesReaderWriter {
public:
    static PlanetPropertyReaderWriter& instance();
private:
    PlanetPropertyReaderWriter();
    virtual ~PlanetPropertyReaderWriter();
private:
    void ReadProperties(const Properties &properties) override;

    void WriteProperties(Properties &properties) override;
};


#endif //CLIONTEST_PLANET_H
