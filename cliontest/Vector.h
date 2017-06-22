//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_VECTOR_H
#define CLIONTEST_VECTOR_H


#include <tuple>
#include <ostream>
#include <cmath>
#include <boost/format.hpp>


template<typename float_t>
class Vector {
public:
    Vector(const float_t& x, const float_t& y) : x_{x}, y_{y} {}
    Vector() {}
    Vector(const Vector&) = default;
    Vector(Vector&&) = default;
    ~Vector() = default;

    Vector& operator=(Vector&&) = default;
    Vector& operator=(const Vector&) = default;

    bool operator<(const Vector &rhs) const {
        return std::tie(x_, y_) < std::tie(rhs.x_, rhs.y_);
    }

    bool operator>(const Vector &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Vector &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Vector &rhs) const {
        return !(*this < rhs);
    }

    bool operator==(const Vector &rhs) const {
        return std::tie(x_, y_) == std::tie(rhs.x_, rhs.y_);
    }

    bool operator!=(const Vector &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector &vector);

    float_t getX() const {
        return x_;
    }

    void setX(const float_t& x_) {
        Vector::x_ = x_;
    }

    float_t getY() const {
        return y_;
    }

    void setY(const float_t& y_) {
        Vector::y_ = y_;
    }

    Vector operator+(const Vector& v) const { return Vector(this->x_ + v.x_, this->y_ + v.y_); }
    Vector& operator+=(const Vector& v) { this->x_ += v.x_; this->y_ += v.y_; return *this; }
    Vector operator-(const Vector& v) const { return Vector(this->x_ - v.x_, this->y_ - v.y_); }
    Vector& operator-=(const Vector& v) { this->x_ -= v.x_; this->y_ -= v.y_; return *this; }
    Vector operator*(const float_t& f) const { return Vector(this->x_ * f, this->y_ * f); }
    Vector& operator*=(const float_t& f) { this->x_ *= f; this->y_ *= f; return *this; }
    float_t operator*(const Vector& v) const { return this->x_ * v.x_ + this->y_ * v.y_; }
    float_t length() const { return std::sqrt( x_ * x_ + y_ * y_ ); }
    Vector& normalize() { *this *= 1/ length(); return *this; }
    Vector normalize_copy() { auto f = 1/ length(); return Vector(x_ * f, y_ * f); }
    float_t dist(const Vector& v) const { return (*this - v).length(); }
    Vector orthogonal_copy() const { return Vector(y_, -x_); }
    Vector& orthogonal() { std::tie(x_, y_) = std::tuple<decltype(x_), decltype(y_)>(y_, -x_); return *this; }

private:
    float_t x_ { 0.0 }, y_{ 0.0 };
};


template<typename float_t>
std::ostream &operator<<(std::ostream &os, const Vector<float_t> &vector) {
    //os << "(x_: " << vector.x_ << ", y_: " << vector.y_ << ")";
    os << boost::format{"(%f6.3, %f6.3)"} % vector.x_ % vector.y_;
    return os;
}

#endif //CLIONTEST_VECTOR_H
