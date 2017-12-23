#include <iostream>
#include "Vec2D.h"

using coord_t = int;
using vec_t = Vec2D<coord_t>;

struct Rect {
    explicit Rect(const vec_t &p1 = vec_t(), const vec_t &p2 = vec_t()) noexcept : p1_(p1), p2_(p2) {
        p1_.assert_smaller(p2_);
    }

    Rect(const Rect&) noexcept = default;

    ~Rect() noexcept = default;

    coord_t Area() const noexcept {
        auto v = p2_ - p1_;
        return v.getX() * v.getY();
    }
    coord_t Width() const noexcept {
        return p2_.getX() - p1_.getX();
    }
    coord_t Height() const noexcept {
        return p2_.getY() - p1_.getY();
    }
    vec_t Center() const noexcept {
        return p1_ + vec_t(Width()/2, Height()/2);
    }

    bool operator==(const Rect &rhs) const noexcept {
        return std::tie(p1_, p2_) == std::tie(rhs.p1_, rhs.p2_);
    }

    bool operator!=(const Rect &rhs) const noexcept {
        return !(rhs == *this);
    }

    bool Interset(const Rect& r) const noexcept {
        return p2_.getX() >= r.p1_.getX() && p2_.getX() <= r.p2_.getX()
                && p2.getY() >= r.p1_.getY() && p2_.getY() <= r.p2_.getY();
    }

    bool Contains(const Rect& r) const noexcept {
        return p1_.getX() <= r.p1_.getX() && p1_.getY() <= r.p1_.getY()
                && p2_.getX() >= r.p2_.getX() && p2_.getY() >= r.p2_.getY();
    }

    Rect Intersection(const Rect& r) const noexcept {

    }

    vec_t p1_, p2_;
};

class RTree {
public:
    using elt_t = std::pair<Rect, size_t>;
    void Add(const elt_t&);
    bool FindContainingElt(const vec_t&, elt_t*);
private:
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}