//
// Created by Arnold Müller on 17.12.17.
//

#ifndef RANGEMAP_CRECT_H
#define RANGEMAP_CRECT_H

#include <tuple>
#include <algorithm>
#include "Point.h"
#include <type_traits>
#include <cstdlib>
#include <ostream>

struct Rect final
{
    using point_t = Point<>;
	enum class ConstructionType { unchecked };
	explicit Rect(const point_t& p1, const point_t& p2) noexcept
	: p1_{p1}, p2_{p2}
	{
		point_t::assert_top_left(p1_, p2_);
	}
	explicit Rect(ConstructionType ct = ConstructionType::unchecked, const point_t& p1 = point_t(), const point_t& p2 = point_t()) noexcept
	: p1_{p1}, p2_{p2}
	{}
	Rect(const Rect&) noexcept = default;
	~Rect() noexcept = default;
	Rect& operator=(const Rect& r) noexcept = default;
	bool operator==(const Rect& r) const noexcept { return std::tie(p1_, p2_) == std::tie(r.p1_, r.p2_); }
	bool contains(const point_t& p) const noexcept {
		return p1_.is_top_left(p) && p.is_top_left(p2_);
	}
	bool contains(const Rect& r) const noexcept {
		return p1_.is_top_left(r.p1_) && r.p2_.is_top_left(p2_);
	}
	bool intersects(const Rect& r) const noexcept {
		return p1_.is_top_left(r.p2_) && r.p2_.is_top_left(p1_);
	}
	static Rect combination(const Rect& r1, const Rect& r2) {
		point_t p1(
			std::min(r1.p1_.x_, r2.p1_.x_),
			std::min(r1.p1_.y_, r2.p1_.y_)
			);
		point_t p2(
			std::max(r1.p2_.x_, r2.p2_.x_),
			std::max(r1.p2_.y_, r2.p2_.y_)
			);
		return Rect(ConstructionType::unchecked, p1, p2);
	}
	static std::pair<bool, Rect> intersection(const Rect& r1, const Rect& r2) {
		point_t p1(std::max(r1.p1_.x_, r2.p1_.x_), std::max(r1.p1_.y_, r2.p1_.y_));
		point_t p2(std::min(r1.p2_.x_, r2.p2_.x_), std::min(r1.p2_.y_, r2.p2_.y_));
		bool intersect = p1.is_top_left(p2);
		Rect r;
		if (intersect) {
			r.p1_ = p1;
			r.p2_ = p2;
		}
		return std::make_pair(intersect, r);
	}
	point_t::value_type area() const {
        auto p = p2_ - p1_;
        if constexpr (std::is_integral<point_t::value_type>::value)
            p += point_t(1, 1);
        return std::abs(p.x_ * p.y_);
    }
	point_t::value_type width() const {
        auto ret = p2_.x_ - p1_.x_;
        if constexpr (std::is_integral<point_t::value_type>::value)
            ret += 1;
        return ret;
    }
	point_t::value_type height() const {
        auto ret = p2_.y_ - p1_.y_;
        if constexpr (std::is_integral<point_t::value_type>::value)
            ret += 1;
        return ret;
    }

    friend std::ostream &operator<<(std::ostream &os, const Rect &rect) {
        os << "[" << rect.p1_ << ", " << rect.p2_ << "]";
        return os;
    }

    point_t p1_, p2_;
};



#endif //RANGEMAP_CRECT_H
