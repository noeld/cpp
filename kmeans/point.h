#pragma once

#include<array>
#include<iostream>

using elt_type = float;
using point_type = std::array<elt_type, 2>;

struct center_info
{
	point_type point_;
	size_t count_ {0};
};

point_type::value_type distance(point_type const & a, point_type const & b);

std::ostream& operator << (std::ostream&, point_type const &);

point_type& operator += (point_type& a, point_type const & b);

point_type& operator /= (point_type& a, elt_type const & b);
