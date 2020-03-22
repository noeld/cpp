#pragma once

#include<array>
#include<iostream>
#include<vector>

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
        auto it = t.begin();
        //for (auto const& e : t)
        if (it != t.end())
        {
            for (size_t i = 0; i < std::tuple_size<point_type>::value; ++i)
            {
                min[i] = max[i] = (*it)[i];
            }
            ++it;
        }
        for(;it != t.end();++it)
        {
            for (size_t i = 0; i < std::tuple_size<point_type>::value; ++i)
            {
                min[i] = std::min(min[i], (*it)[i]);
                //min[1] = std::min(min[1], e[1]);
                max[i] = std::max(max[i], (*it)[i]);
                //max[1] = std::max(max[1], e[1]);
            }
        }
	}

    bounding_rect(bounding_rect const&) = default;

    friend std::ostream& operator<<(std::ostream& o, bounding_rect const& r);

    elt_type min[2]{ std::numeric_limits<elt_type>::max(), std::numeric_limits<elt_type>::max() };
    elt_type max[2]{ std::numeric_limits<elt_type>::min(), std::numeric_limits<elt_type>::min() };
};

class kd_tree
{
public:
    explicit kd_tree() = default;

    template<typename T>
    int build(T begin, T end);

    std::tuple<point_type, size_t> nearest(point_type const& p) const;

private:
    void bulk_load(size_t depth, std::vector <point_type> const & points);

    constexpr static unsigned IDX_NONE = std::numeric_limits<unsigned>::max();
    struct kd_node { point_type point_; size_t original_idx_{ 0 }; unsigned left_idx_{ IDX_NONE }; unsigned right_idx_{ IDX_NONE }; };

    std::vector<kd_node> nodes_;
    size_t root_idx_{ 0 };
};

template<typename T>
inline int kd_tree::build(T begin, T end)
{
    return 0;
}
