#include "point.h"

point_type::value_type distance(point_type const & a, point_type const & b)
{
	auto x = b[0] - a[0];
	auto y = b[1] - a[1];
	return x * x + y * y;
}


std::ostream& operator << (std::ostream& o, point_type const & p)
{
	return o << "(" << p[0] << ", " << p[1] << ")";
}

point_type& operator += (point_type& a, point_type const & b)
{
	a[0] += b[0];
	a[1] += b[1];
	return a;
}

point_type& operator /= (point_type& a, elt_type const & b)
{
	a[0] /= b;
	a[1] /= b;
	return a;
}