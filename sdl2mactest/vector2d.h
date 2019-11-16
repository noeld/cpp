//
// Created by Arnold Müller on 11.02.17.
//

#pragma once

#include <tuple>
#include <ostream>
#include <cmath>
//#include <boost/format.hpp>


template<typename float_t>
class vector2d {
public:
    vector2d(const float_t& x, const float_t& y) : x_{x}, y_{y} {}
    vector2d() {}
    vector2d(const vector2d&) = default;
    vector2d(vector2d&&) = default;
    ~vector2d() = default;

    vector2d& operator=(vector2d&&) = default;
    vector2d& operator=(const vector2d&) = default;

    bool operator<(const vector2d &rhs) const {
        return std::tie(x_, y_) < std::tie(rhs.x_, rhs.y_);
    }

    bool operator>(const vector2d &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const vector2d &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const vector2d &rhs) const {
        return !(*this < rhs);
    }

    bool operator==(const vector2d &rhs) const {
        return std::tie(x_, y_) == std::tie(rhs.x_, rhs.y_);
    }

    bool operator!=(const vector2d &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const vector2d &vector2d);

    float_t getX() const {
        return x_;
    }

    void setX(const float_t& x_) {
        vector2d::x_ = x_;
    }

    float_t getY() const {
        return y_;
    }

    void setY(const float_t& y_) {
        vector2d::y_ = y_;
    }

    vector2d operator+(const vector2d& v) const { return vector2d(this->x_ + v.x_, this->y_ + v.y_); }
    vector2d& operator+=(const vector2d& v) { this->x_ += v.x_; this->y_ += v.y_; return *this; }
    vector2d operator-(const vector2d& v) const { return vector2d(this->x_ - v.x_, this->y_ - v.y_); }
    vector2d& operator-=(const vector2d& v) { this->x_ -= v.x_; this->y_ -= v.y_; return *this; }
    vector2d operator*(const float_t& f) const { return vector2d(this->x_ * f, this->y_ * f); }
    vector2d& operator*=(const float_t& f) { this->x_ *= f; this->y_ *= f; return *this; }
    float_t operator*(const vector2d& v) const { return this->x_ * v.x_ + this->y_ * v.y_; }
    float_t length() const { return std::sqrt( x_ * x_ + y_ * y_ ); }
    float_t length_norm() const { return x_ * x_ + y_ * y_ ; }
    vector2d& normalize() { *this *= 1/ length(); return *this; }
    vector2d normalize_copy() { auto f = 1/ length(); return vector2d(x_ * f, y_ * f); }
    float_t dist(const vector2d& v) const { return (*this - v).length(); }
    vector2d orthogonal_copy() const { return vector2d(y_, -x_); }
    vector2d& orthogonal() { std::tie(x_, y_) = std::tuple<decltype(x_), decltype(y_)>(y_, -x_); return *this; }

private:
    float_t x_ { 0.0 }, y_{ 0.0 };
};


// template<typename float_t>
// std::ostream &operator<<(std::ostream &os, const vector2d<float_t> &vector) {
//     //os << "(x_: " << vector.x_ << ", y_: " << vector.y_ << ")";
//     os << boost::format{"(%f6.3, %f6.3)"} % vector.x_ % vector.y_;
//     return os;
// }

