#pragma once

#include<array>
#include<iostream>

using elt_type = double;
using point_type = std::array<elt_type, 2>;

struct center_info
{
	point_type point_;
	size_t count_ {0};
};

point_type::value_type distance2(point_type const & a, point_type const & b);

std::ostream& operator << (std::ostream&, point_type const &);

point_type& operator += (point_type& a, point_type const & b);

point_type& operator -= (point_type& a, point_type const& b);

point_type& operator /= (point_type& a, elt_type const & b);

point_type& operator *= (point_type& a, elt_type const & b);

point_type operator * (point_type const& a, elt_type const & b);

struct bounding_rect
{
    bounding_rect() = default;

	template<typename T>
	explicit bounding_rect(T const& t)
	{
        for (auto const& e : t)
        {
            min[0] = std::min(min[0], e[0]);
            min[1] = std::min(min[1], e[1]);
            max[0] = std::max(max[0], e[0]);
            max[1] = std::max(max[1], e[1]);
        }
	}

    bounding_rect(bounding_rect const&) = default;

    friend std::ostream& operator<<(std::ostream& o, bounding_rect const& r);

    elt_type min[2]{ std::numeric_limits<elt_type>::max(), std::numeric_limits<elt_type>::max() };
    elt_type max[2]{ std::numeric_limits<elt_type>::min(), std::numeric_limits<elt_type>::min() };
};