#pragma once

struct Rect final
{
	enum class ConstructionType { unchecked };
	explicit Rect(const Point& p1, const Point& p2) noexcept
	: p1_{p1}, p2_{p2}
	{
		Point::assert_top_left(p1_, p2_);
	}
	explicit Rect(ConstructionType ct = ConstructionType::unchecked, const Point& p1 = Point(), const Point& p2 = Point()) noexcept
	: p1_{p1}, p2_{p2}
	{}
	Rect(const Rect&) noexcept = default;
	~Rect() noexcept = default;
	Rect& operator=(const Rect& r) noexcept = default;
	bool operator==(const Rect& r) const noexcept { return std::tie(p1_, p2_) == std::tie(r.p1_, r.p2_); }
	bool contains(const Point& p) const noexcept {
		return p1_.is_top_left(p) && p.is_top_left(p2_);
	}
	bool contains(const Rect& r) const noexcept {
		return p1_.is_top_left(r.p1_) && r.p2_.is_top_left(p2_);
	}
	bool intersects(const Rect& r) const noexcept {
		return p1_.is_top_left(r.p2_) && r.p2_.is_top_left(p1_);
	}
	static Rect combination(const Rect& r1, const Rect& r2) {
		Point p1(
			std::min(r1.p1_.x_, r2.p1_.x_),
			std::min(r1.p1_.y_, r2.p1_.y_)
			);
		Point p2(
			std::max(r1.p2_.x_, r2.p2_.x_),
			std::max(r1.p2_.y_, r2.p2_.y_)
			);
		return Rect(ConstructionType::unchecked, p1, p2);
	}
	static std::pair<bool, Rect> intersection(const Rect& r1, const Rect& r2) {
		Point p1(std::max(r1.p1_.x_, r2.p1_.x_), std::max(r1.p1_.y_, r2.p1_.y_));
		Point p2(std::min(r1.p2_.x_, r2.p2_.x_), std::min(r1.p2_.y_, r2.p2_.y_));
		bool intersect = p1.is_top_left(p2);
		Rect r;
		if (intersect) {
			r.p1_ = p1;
			r.p2_ = p2;
		}
		return std::make_pair(intersect, r);
	}
	Point::value_type area() const { auto p = p2_ - p1_; return std::abs(p.x_ * p.y_); }
	Point::value_type width() const { return p2_.x_ - p1_.x_ + 1; }
	Point::value_type height() const { return p2_.y_ - p1_.y_ + 1; }
	Point p1_, p2_;
};
