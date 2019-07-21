#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <limits>

struct point {
    int x_ {0};
    int y_ {0};
    point() = default;
    explicit point(int x, int y) : x_{x}, y_{y}{}
    point(const point&) = default;
    point& operator=(const point&) = default;
    int operator[](size_t i) const noexcept {
        return (i == 0) ? x_ : y_;
    }
    friend std::ostream& operator<<(std::ostream& o, const point& p) {
        return o << '(' << p.x_ << "; " << p.y_ << ')';
    }
};

using point_vector = std::vector<point>;
using point_it     = point_vector::iterator;

std::ostream& operator<<(std::ostream& o, const point_vector& pv) {
    o << '{';
    if (pv.size() > 0)
        o << pv[0];
    for (point_vector::size_type i = 1; i < pv.size(); ++i) {
        o << "; " << pv[i];
    }
    o << '}';
    return o;
}

struct rect {
    point min_, max_;
    friend std::ostream& operator<<(std::ostream& o, const rect& p) {
        return o << '[' << p.min_ << "; " << p.max_ << ']';
    }
};

template<typename T>
T median_le(T start, T end, size_t dim) {
    if (start == end)
        return end;
    if (start + 1 == end)
        return start;
    T m = start + std::distance(start, end)/2;
     // std::cout << std::distance(start, m) << ": " << *m << " . ";
    auto acc = [dim](const typename T::value_type& a, const typename T::value_type& b) -> bool { return a[dim] < b[dim]; };
    std::nth_element(start, m, end, acc);
    for(auto m2 = m + 1; m2 != end && (*m2)[dim] == (*m)[dim]; ++m2, ++m);
    // std::cout << "=" << (*m)[dim] << " - " << std::distance(start, m);
    return m;
}

struct kdtree {
    static const size_t NOCHILD = std::numeric_limits<size_t>::max();
    struct kdnode {
        point p_;
        int operator[](size_t i) const noexcept {
            return p_[i];
        }
        size_t left_ { NOCHILD }, right_ {NOCHILD};
        unsigned depth_ { 0 };
        unsigned dim() const noexcept { return depth_ % 2; }
        friend std::ostream& operator<<(std::ostream& o, const kdtree::kdnode& n) {
            o << "{" << n.p_ << ", " ;
            if (n.left_ == NOCHILD)
                o << "-";
            else
                o << n.left_;
            o << ", ";
            if (n.right_ == NOCHILD)
                o << "-";
            else
                o << n.right_;
            o << ", ";
            o << n.depth_ << "}";
            return o;
        }
    };
    void build(const std::vector<point>& points) {
        t_.resize(points.size());
        auto pit = points.cbegin();
        auto pend = points.cend();
        auto it = t_.begin();
        while(pit != pend) {
            it->p_ = *pit;
            ++pit;
            ++it;
        }
        root_ = buildtree(t_.begin(), t_.end(), 0);
    }
    void print() const {
        std::cout << std::endl;
        for (size_t i = 0; i < t_.size(); ++i) {
            std::cout << i << ": " << t_[i] << std::endl;
        }
        std::cout << "root: " << root_ << std::endl;
    }
    private:
        std::vector<kdtree::kdnode> t_;
        size_t root_ { 0 };
protected:
    size_t buildtree(decltype(t_.begin()) begin, decltype(t_.begin()) end, unsigned depth) {
        if (begin == end)
            return NOCHILD;
        auto m = median_le(begin, end, depth % 2);
        auto ret = std::distance(t_.begin(), m);
        auto dp1 = depth + 1;
        m->depth_ = depth;
        auto lm = buildtree(begin, m, dp1);
        auto rm = buildtree(m + 1, end, dp1);
        m->left_ = lm;
        m->right_ = rm;
        return ret;
    }

};



int main(int argc, char const *argv[])
{
    using namespace std;

    size_t num_points = 10;
    if (argc > 1)
        num_points = std::stoul(argv[1]);
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, num_points);
    point_vector points;
    for(size_t i = 0; i < num_points; ++i) {
        points.emplace_back(dist(rd), dist(rd));
    }
    std::cout << points << std::endl;
    // median_le(points.begin(), points.end(), 0);
    // std::cout << points << std::endl;
    // median_le(points.begin(), points.end(), 1);
    // std::cout << points << std::endl;
    //std::vector<std::reference_wrapper<point>> idxx(points.begin(), points.end());
    //std::vector<std::reference_wrapper<point>> idxy(points.begin(), points.end());
    kdtree t;
    t.build(points);
    t.print();
    return 0;
}