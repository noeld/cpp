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
    // if both planets overlapp, relocate them along their distance vector
    auto relocate_dist = a.getR() + b.getR() - dist_length;
    if (relocate_dist > 0.0) {
        auto rel_a = a.mass_ / (a.mass_ + b.mass_);
        auto rel_b = b.mass_ / (a.mass_ + b.mass_);
        a.pos_ += dist_vec * (rel_a * relocate_dist);
        b.pos_ += dist_vec * (-rel_b * relocate_dist);
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
        return 0;
    }
    auto ortho = dist_vec.orthogonal_copy();
    auto ortho_force_a  = ortho * ( ortho * a.speed_ );
    auto direct_force_a = dist_vec * ( dist_vec * a.speed_ );
    auto ortho_force_b  = ortho * ( ortho * b.speed_ );
    auto direct_force_b = dist_vec * ( dist_vec * b.speed_ );

    auto mv1mv2 = direct_force_a * a.mass_ + direct_force_b * b.mass_;
    a.speed_ = ortho_force_a + (mv1mv2 - (direct_force_a - direct_force_b) * (b.mass_ * k)) * ( 1 / (a.mass_ + b.mass_));
    b.speed_ = ortho_force_b + (mv1mv2 - (direct_force_b - direct_force_a) * (a.mass_ * k)) * ( 1 / (a.mass_ + b.mass_));

    return 0;
}


void Planet::Join(Planet &a, Planet &b) {
    Planet* big = &a;
    Planet* small = &b;
    if (small->getMass() > big->getMass()) {
        std::swap(big, small);
    }
    auto mm = big->getMass() + small->getMass();
    auto dist_vec = big->pos_ - small->pos_;
    auto f = small->getMass() / mm;
    auto pp = big->pos_ + dist_vec * -f;
    auto ss = big->speed_ * (big->getMass() / mm) + small->speed_ * (small->getMass() / mm);

    big->setRgb_(Planet::BlendWeightedRGB(big->getRgb_(), big->getMass(), small->getRgb_(), small->getMass()));
    big->setPos(pp);
    big->setSpeed(ss);
    big->setActive(true);
    small->setActive(false);
    big->setMass(mm);
/*
var big = this;
			var small = o;
			if (small.m > this.m) {
				big = o;
				small = this;
			}
			var mm = big.m + small.m;
			var rr = this.radiusFromMass(mm);
			var d = big.pos.sub_copy(small.pos);
			var dmagn = d.length();
			var f = small.m / mm;
			var pp = big.pos.add_copy(d.mul(-f));
			var ss = big.speed.mul_copy(big.m / mm).add(small.speed.mul_copy(small.m / mm));
			big.m = mm;
			big.r = rr;
			big.pos = pp;
			big.speed = ss;
			big.active = 1;
			small.active = 0;
 * */
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
