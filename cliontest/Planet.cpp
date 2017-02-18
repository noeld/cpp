//
// Created by Arnold Müller on 11.02.17.
//

#include "Planet.h"

Planet::Planet() {
    this->r_ = Planet::RadiusFromMass(mass_);
}

double Planet::RadiusFromMass(const double& m) {
    return std::pow( m/M_PI, 1.0/6.0);
}

Planet::Planet(const Vector &pos_, const Vector &speed_, double mass_, bool active = true) : pos_(pos_),
                                                                                                 speed_(speed_),
                                                                                                 mass_(mass_),
                                                                                                 active(active) {
    this->r_ = Planet::RadiusFromMass(mass_);
}

Planet::~Planet() {

}

unsigned Planet::Collide(Planet &a, Planet &b, const double& k, const double& d) {
    auto speed_delta = a.speed_ - b.speed_;
    if (speed_delta.magn() <= d) {
        Planet::Join(a, b);
        return 1;
    }
    auto dist_vec = (a.pos_ - b.pos_);
    dist_vec.normalize();
    if (dist_vec * speed_delta > 0) {
        // Distance between Planets already increasing
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
			var dmagn = d.magn();
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

#define EXTRACTCOLOR(a, s, w) (static_cast<double>(((a) >> (s)) & 0xff) * w)
unsigned Planet::BlendWeightedRGB(unsigned x, double wa, unsigned y, double wb) {
    unsigned r = static_cast<unsigned>((EXTRACTCOLOR(x, 16, wa) + EXTRACTCOLOR(y, 16, wb)) / (wa + wb)) & 0xff;
    unsigned g = static_cast<unsigned>((EXTRACTCOLOR(x,  8, wa) + EXTRACTCOLOR(y,  8, wb)) / (wa + wb)) & 0xff;
    unsigned b = static_cast<unsigned>((EXTRACTCOLOR(x,  0, wa) + EXTRACTCOLOR(y,  0, wb)) / (wa + wb)) & 0xff;
    return (r << 16) + (g << 8) + b ;
}
