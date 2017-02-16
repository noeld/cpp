//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_VECTOR_H
#define CLIONTEST_VECTOR_H


#include <tuple>
#include <ostream>
#include <cmath>

class Vector {
public:
    Vector(const double& x, const double& y);
    Vector();
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

    double getX() const {
        return x_;
    }

    void setX(const double& x_) {
        Vector::x_ = x_;
    }

    double getY() const {
        return y_;
    }

    void setY(const double& y_) {
        Vector::y_ = y_;
    }

    Vector operator+(const Vector& v) const { return Vector(this->x_ + v.x_, this->y_ + v.y_); }
    Vector& operator+=(const Vector& v) { this->x_ += v.x_; this->y_ += v.y_; return *this; }
    Vector operator-(const Vector& v) const { return Vector(this->x_ - v.x_, this->y_ - v.y_); }
    Vector& operator-=(const Vector& v) { this->x_ -= v.x_; this->y_ -= v.y_; return *this; }
    Vector operator*(const double& f) const { return Vector(this->x_ * f, this->y_ * f); }
    Vector& operator*=(const double& f) { this->x_ *= f; this->y_ *= f; return *this; }
    double operator*(const Vector& v) const { return this->x_ * v.x_ + this->y_ * v.y_; }
    double magn() const { return std::sqrt( x_ * x_ + y_ * y_ ); }
    Vector& normalize() { *this *= 1/magn(); return *this; }
    Vector normalize_copy() { auto f = magn(); return Vector(x_ * f, y_ * f); }
    double dist(const Vector& v) const { return (*this - v).magn(); }
    Vector orthogonal_copy() const { return Vector(y_, -x_); }
    Vector& orthogonal() { std::tie(x_, y_) = std::tuple<decltype(x_), decltype(y_)>(y_, -x_); return *this; }

private:
    double x_ { 0.0 }, y_{ 0.0 };
};


#endif //CLIONTEST_VECTOR_H
