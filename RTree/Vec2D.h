//
// Created by Arnold Müller on 21.10.17.
//

#ifndef RTREE_VECTOR_H
#define RTREE_VECTOR_H



#include <tuple>
#include <ostream>
#include <cmath>
#include <boost/format.hpp>


template<typename float_t>
class Vec2D {
public:
    explicit Vec2D(const float_t& x, const float_t& y) noexcept : x_{x}, y_{y} {}
    Vec2D() noexcept {}
    Vec2D(const Vec2D&) noexcept = default;
    Vec2D(Vec2D&&) = default;
    ~Vec2D() = default;

    //Vec2D& operator=(Vec2D&&) = default;
    Vec2D& operator=(const Vec2D&) noexcept = default;

    bool operator<(const Vec2D &rhs) const {
        return std::tie(x_, y_) < std::tie(rhs.x_, rhs.y_);
    }

    bool operator>(const Vec2D &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Vec2D &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Vec2D &rhs) const {
        return !(*this < rhs);
    }

    bool operator==(const Vec2D &rhs) const {
        return std::tie(x_, y_) == std::tie(rhs.x_, rhs.y_);
    }

    bool operator!=(const Vec2D &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vec2D &vector);

    float_t getX() const {
        return x_;
    }

    void setX(const float_t& x_) {
        Vec2D::x_ = x_;
    }

    float_t getY() const {
        return y_;
    }

    void setY(const float_t& y_) {
        Vec2D::y_ = y_;
    }

    Vec2D   operator+ (const Vec2D& v) const { return Vec2D(this->x_ + v.x_, this->y_ + v.y_); }
    Vec2D&  operator+=(const Vec2D& v) { this->x_ += v.x_; this->y_ += v.y_; return *this; }
    Vec2D   operator- (const Vec2D& v) const { return Vec2D(this->x_ - v.x_, this->y_ - v.y_); }
    Vec2D&  operator-=(const Vec2D& v) { this->x_ -= v.x_; this->y_ -= v.y_; return *this; }
    Vec2D   operator* (const float_t& f) const { return Vec2D(this->x_ * f, this->y_ * f); }
    Vec2D&  operator*=(const float_t& f) { this->x_ *= f; this->y_ *= f; return *this; }
    float_t operator* (const Vec2D& v) const { return this->x_ * v.x_ + this->y_ * v.y_; }
    float_t length() const { return std::sqrt( x_ * x_ + y_ * y_ ); }
    Vec2D&  normalize() { *this *= 1/ length(); return *this; }
    Vec2D   normalize_copy() { auto f = 1/ length(); return Vec2D(x_ * f, y_ * f); }
    float_t dist(const Vec2D& v) const { return (*this - v).length(); }
    Vec2D   orthogonal_copy() const { return Vec2D(y_, -x_); }
    Vec2D&  orthogonal() { std::tie(x_, y_) = std::tuple<decltype(x_), decltype(y_)>(y_, -x_); return *this; }

    void assert_smaller(Vec2D& v) noexcept {
        if ( x_ > v.x_ )
            std::swap(x_, v.x_);
        if ( y_ > v.y_ )
            std::swap(y_, v.y_);
    }

private:
    float_t x_ { 0.0 }, y_{ 0.0 };
};


template<typename float_t>
std::ostream &operator<<(std::ostream &os, const Vec2D<float_t> &vector) {
    //os << "(x_: " << vector.x_ << ", y_: " << vector.y_ << ")";
    os << boost::format{"(%f6.3, %f6.3)"} % vector.x_ % vector.y_;
    return os;
}


#endif //RTREE_VECTOR_H
