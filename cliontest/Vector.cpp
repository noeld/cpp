//
// Created by Arnold Müller on 11.02.17.
//

#include "Vector.h"
#include <boost/format.hpp>

Vector::Vector(const double& x, const double& y) : x_(x), y_(y) {}

Vector::Vector() {}

std::ostream &operator<<(std::ostream &os, const Vector &vector) {
    //os << "(x_: " << vector.x_ << ", y_: " << vector.y_ << ")";
    os << boost::format{"(%f6.3, %f6.3)"} % vector.x_ % vector.y_;
    return os;
}
