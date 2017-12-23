//
// Created by Arnold Müller on 17.12.17.
//

#ifndef RANGEMAP_TRANSFORM_H
#define RANGEMAP_TRANSFORM_H


#include <cmath>
#include <ostream>
#include "Point.h"
#include "CRect.h"
#include <boost/format.hpp>

class Transform final {
public:
    explicit Transform() noexcept {}

    explicit Transform(double a11, double a12, double a21, double a22, double t1, double t2)
            : a11_(a11), a12_(a12), a21_(a21), a22_(a22), t1_(t1), t2_(t2) {}

    Transform(const Transform&) noexcept = default;
    ~Transform() noexcept = default;

    static Transform Translate(double x, double y) {
        return Transform(1.0, 0.0, 0.0, 1.0, x, y);
    }
    static Transform Translate(const Point<> p) {
        return Transform::Translate(p.x_, p.y_);
    }
    //Transform& translate(double x, double y) {}
    static Transform Scale(double x, double y) {
        return Transform(x, 0.0, 0.0, y, 0.0, 0.0);
    }
    static Transform Scale(const Point<> p) {
        return Transform::Scale(p.x_, p.y_);
    }
    static Transform Rotate(double a) {
        return Transform(std::cos(a), std::sin(a), -std::sin(a), std::cos(a), 0, 0);
    }

    friend Point<> operator*(const Transform& t, const Point<>& p) {
        if constexpr(std::is_integral<Point<>::value_type>::value)
            return Point<>(
                    std::lrint(p.x_ * t.a11_ + p.y_ * t.a12_ + t.t1_),
                    std::lrint(p.x_ * t.a21_ + p.y_ * t.a22_ + t.t2_)
            );
        else return Point<>(
                    p.x_ * t.a11_ + p.y_ * t.a12_ + t.t1_,
                    p.x_ * t.a21_ + p.y_ * t.a22_ + t.t2_
            );
    }
    friend Rect operator*(const Transform& t, const Rect& r) {
        return Rect(Rect::ConstructionType::unchecked, t * r.p1_, t * r.p2_);
    }
    friend Transform operator*(const Transform& a, const Transform& b) {
        return Transform(
                a.a11_ * b.a11_ + a.a12_ * b.a21_
              , a.a11_ * b.a12_ + a.a12_ * b.a22_
              , a.a21_ * b.a11_ + a.a22_ * b.a21_
              , a.a21_ * b.a12_ + a.a22_ * b.a22_
              , a.a11_ * b.t1_  + a.a12_ * b.t2_   + a.t1_
              , a.a21_ * b.t1_  + a.a22_ * b.t2_   + a.t2_
        );
    }

    friend std::ostream &operator<<(std::ostream &os, const Transform &transform) {
        os << boost::format{"|%5.1f %5.1f %5.1f|\n|%5.1f %5.1f %5.1f|\n"
        //                    "|%5.1f %5.1f %5.1f|"
        }
                % transform.a11_ % transform.a12_ % transform.t1_
                % transform.a21_ % transform.a22_ % transform.t2_;
//              % 0.0 % 0.0 % 1.0;
//        os << "a11_: " << transform.a11_ << " a12_: " << transform.a12_ << " a21_: " << transform.a21_ << " a22_: "
//           << transform.a22_ << " t1_: " << transform.t1_ << " t2_: " << transform.t2_;
        return os;
    }

    double a11_{1.0}, a12_{0}, a21_{0}, a22_{1}, t1_{0}, t2_{0};
};


#endif //RANGEMAP_TRANSFORM_H
