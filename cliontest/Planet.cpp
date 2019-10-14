//
// Created by Arnold Müller on 11.02.17.
//

#include "Planet.h"

Planet::float_t Planet::universe_planet_density { 5.0 };

Planet::Planet() {
    this->r_ = Planet::RadiusFromMass(mass_);
}

Planet::float_t Planet::RadiusFromMass(const float_t& m) {
    return std::pow( m/M_PI, 1.0/Planet::universe_planet_density);
}

Planet::Planet(const Vector<float_t> &pos_, const Vector<float_t> &speed_, float_t mass_, bool active = true) : pos_(pos_),
                                                                                                 speed_(speed_),
                                                                                                 mass_(mass_),
                                                                                                 active(active) {
    this->r_ = Planet::RadiusFromMass(mass_);
}

Planet::~Planet() {

}

unsigned Planet::Collide(Planet &a, Planet &b, const float_t& k, const float_t& d) {
    auto speed_delta = a.speed_ - b.speed_;
    if (speed_delta.length() <= d) {
        Planet::Join(a, b);
        return 1;
    }
    auto dist_vec = (a.pos_ - b.pos_);
    auto dist_length = dist_vec.length();
    dist_vec.normalize();
    if (std::isnan(dist_vec.getX()) || std::isnan(dist_vec.getY())) {
        // Distance between planets is 0.0
        Planet::Join(a, b);
        return 1;
    }
//    auto r2 = a.getR() + b.getR() - dist_length;
//    if (r2 > 0) {
//        //Increase distance if planets overlapp
//        a.pos_ += dist_vec * ( r2 * b.getMass() / (a.getMass() + b.getMass()));
//        b.pos_ -= dist_vec * ( r2 * a.getMass() / (a.getMass() + b.getMass()));
//    }
    if (dist_vec * speed_delta > 0) {
        // Distance between Planets already increasing
        //dist_vec *= -1;
    } else {
        auto ortho = dist_vec.orthogonal_copy();
        auto ortho_force_a  = ortho * ( ortho * a.speed_ );
        auto direct_force_a = dist_vec * ( dist_vec * a.speed_ );
        auto ortho_force_b  = ortho * ( ortho * b.speed_ );
        auto direct_force_b = dist_vec * ( dist_vec * b.speed_ );

        auto mv1mv2 = direct_force_a * a.mass_ + direct_force_b * b.mass_;
        a.speed_ = ortho_force_a + (mv1mv2 - (direct_force_a - direct_force_b) * (b.mass_ * k)) * ( 1 / (a.mass_ + b.mass_));
        b.speed_ = ortho_force_b + (mv1mv2 - (direct_force_b - direct_force_a) * (a.mass_ * k)) * ( 1 / (a.mass_ + b.mass_));
    }
    // if both planets overlapp, relocate them along their distance vector
    auto relocate_dist = a.getR() + b.getR() - dist_length;
    if (relocate_dist > 0.0) {
        auto rel_a = a.mass_ / (a.mass_ + b.mass_);
        auto rel_b = b.mass_ / (a.mass_ + b.mass_);
        a.pos_ += dist_vec * (rel_a * relocate_dist);
        b.pos_ += dist_vec * (-rel_b * relocate_dist);
    }
    return 0;
}


void Planet::Join(Planet &a, Planet &b) {
    Planet* big_planet = &a;
    Planet* small_planet = &b;
    if (small_planet->getMass() > big_planet->getMass()) {
        std::swap(big_planet, small_planet);
    }
    auto mm = big_planet->getMass() + small_planet->getMass();
    auto dist_vec = big_planet->pos_ - small_planet->pos_;
    auto f = small_planet->getMass() / mm;
    auto pp = big_planet->pos_ + dist_vec * -f;
    auto ss = big_planet->speed_ * (big_planet->getMass() / mm) + small_planet->speed_ * (small_planet->getMass() / mm);

    big_planet->setRgb_(Planet::BlendWeightedRGB(big_planet->getRgb_(), big_planet->getMass(), small_planet->getRgb_(), small_planet->getMass()));
    big_planet->setPos(pp);
    big_planet->setSpeed(ss);
    big_planet->setActive(true);
	small_planet->setActive(false);
    big_planet->setMass(mm);

}

#define EXTRACTCOLOR(a, s, w) (static_cast<float_t>(((a) >> (s)) & 0xff) * w)
unsigned Planet::BlendWeightedRGB(unsigned x, float_t wa, unsigned y, float_t wb) {
    unsigned r = static_cast<unsigned>((EXTRACTCOLOR(x, 16, wa) + EXTRACTCOLOR(y, 16, wb)) / (wa + wb)) & 0xff;
    unsigned g = static_cast<unsigned>((EXTRACTCOLOR(x,  8, wa) + EXTRACTCOLOR(y,  8, wb)) / (wa + wb)) & 0xff;
    unsigned b = static_cast<unsigned>((EXTRACTCOLOR(x,  0, wa) + EXTRACTCOLOR(y,  0, wb)) / (wa + wb)) & 0xff;
    return (r << 16) + (g << 8) + b ;
}

PlanetPropertyReaderWriter::~PlanetPropertyReaderWriter() {

}

PlanetPropertyReaderWriter::PlanetPropertyReaderWriter() {}

void PlanetPropertyReaderWriter::ReadProperties(const Properties &properties) {
    Planet::universe_planet_density = properties.universe_planet_density;
}

void PlanetPropertyReaderWriter::WriteProperties(Properties &properties) {
    properties.universe_planet_density = Planet::universe_planet_density;
}

PlanetPropertyReaderWriter &PlanetPropertyReaderWriter::instance() {
    static PlanetPropertyReaderWriter pprw;
    return pprw;
}
