//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_UNIVERSE_H
#define CLIONTEST_UNIVERSE_H


#include <vector>
#include <chrono>
#include "Planet.h"
#include "Properties.h"

class Universe : public PropertiesReaderWriter {
public:
    explicit Universe(size_t);

    virtual ~Universe();

    //using time_point = std::chrono::high_resolution_clock::time_point;
    //using now = std::chrono::high_resolution_clock::now;

    const std::vector<Planet> &getObjects() const {
        return objects_;
    }
    /// Advance universe using delta time
    /// \param t time delta in seconds or fractions thereof
    /// \returns void
    void advance(Planet::float_t t);

    /// Get number of active objects
    /// \returns number of active objects
    size_t getNr_() const { return nr_; }

    void ReadProperties(const Properties &properties) override;

    void WriteProperties(Properties &properties) override;

    void Initialize();
    void Refill();

    size_t getActiveObjects() const {
        return activeObjects;
    }
private:
    std::vector<Planet> objects_;

    size_t activeObjects = 0;

private:

    size_t nr_;
    Planet::float_t universe_g { 6.6740831e-11 * 100e8 };
    Planet::float_t universe_collision_joindist { 0.2 };
    Planet::float_t universe_collision_k { 0.5 };
    unsigned generate_n { 1000 };
    Planet::float_t generate_speed_min { 0 };
    Planet::float_t generate_speed_max { 5 };
    Planet::float_t generate_border_part { 0.05 };
    Planet::float_t generate_mass_min { 50 };
    Planet::float_t generate_mass_max { 10000 };
    Planet::float_t generate_mass_alpha { 0.1 };
    Planet::float_t generate_pos_xmax   { 900.0 };
    Planet::float_t generate_pos_ymax   { 600.0 };


    Planet::float_t   generate_mass_beta  { 9.0 };
};


#endif //CLIONTEST_UNIVERSE_H
