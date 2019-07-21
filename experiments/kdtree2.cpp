// http://www.rosettacode.org/wiki/K-d_tree

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <limits>
// #include <boost/timer/timer.hpp>
#include "timemeasure.h"

struct point {
private:
    int x_ {0};
    int y_ {0};
public:
    point() = default;
    point(int x, int y) : x_{x}, y_{y} {}
    point(const point&) = default;
    const int& x() const noexcept { return x_; }
    int& x() noexcept { return x_; }
    const int& y() const noexcept { return y_; }
    int& y() noexcept { return y_; }

    point& operator=(const point&) = default;
    int operator[](size_t i) const noexcept {
        return (i == 0) ? x_ : y_;
    }
    double distance_norm(const point& other) const {
        double dx = static_cast<double>(x_ - other.x_);
        double dy = static_cast<double>(y_ - other.y_);
        double res = (dx * dx) + (dy * dy);
        if (std::isnan(res)) {
            std::cerr << "nan! for ref: " << (*this) << " other: " << other 
                      << " dx: " << dx << " dy: " << dy 
                      << std::endl;
        }
        return res;
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
        kdnode& operator=(const point& p) {
            p_ = p;
        }
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

    template<typename T>
    void build(const T& points) {
        build(std::begin(points), std::end(points));
    }

    template<typename T>
    void build(T begin, T end) {
        t_.resize(std::distance(begin, end));
        auto it = t_.begin();
        while(begin != end) {
            it->p_ = *begin;
            ++begin;
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
    struct search_def {
        explicit search_def(const point& p) : ref_{p} {}
        search_def(const search_def&) = default;
        search_def& operator=(const search_def&) = default;
        double distance() const noexcept { return std::sqrt(distance_norm_); }
        const point ref_;  // point to search for
        const point* result_ { nullptr };  // closest match found
        double distance_norm_ { std::numeric_limits<double>::infinity() };  // square! of distance
        unsigned nodes_visited_ { 0 };  // number of nodes visited during search
        friend std::ostream& operator<<(std::ostream& o, const search_def& sd) {
            o << "Reference point: " << sd.ref_
              << " Found: ";
            if (sd.result_ == nullptr)
                o << "(none)";
            else 
                o << *sd.result_;
            o << " Distance " << sd.distance()
              << " nodes visited: " << sd.nodes_visited_;
            return o;
        }
    };
    search_def nearest_neighbour(const point& ref) {
        search_def search { ref };
        if ( root_ == NOCHILD )
            return search;
        find_nearest_neighbour(search, root_, 0);
        return search;
    }
    private:
        std::vector<kdtree::kdnode> t_;
        size_t root_ { NOCHILD };
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
    void find_nearest_neighbour(search_def& search, size_t node_idx, unsigned depth) {
        if (node_idx == NOCHILD)
            return;
        ++search.nodes_visited_;
        kdtree::kdnode& n = t_[node_idx];
        auto i = depth % 2;
        auto d = search.ref_.distance_norm(n.p_);
        if (d < search.distance_norm_) {
            search.distance_norm_ = d;
            search.result_   = &n.p_;
        }
        double plane_dist  = n.p_[i] - search.ref_[i];
        double plane_dist2 = plane_dist * plane_dist;
        // current splitting hyperplane is right of ref point -> search left subtree
        // otherwise if hyperplane is left of ref point -> search right subtree
        auto next_node = (plane_dist > 0) ? n.left_ : n.right_;
        find_nearest_neighbour(search, next_node, depth + 1);
        // if best distance is less equal the distance of the hyperplane, we
        // don't need to check other side
        if (plane_dist2 >= search.distance_norm_)
            return;
        // otherwise search other side as well
        next_node = (plane_dist > 0) ? n.right_ : n.left_;
        find_nearest_neighbour(search, next_node, depth + 1);
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
    {
        TimeMeasure timer(true, "generating random points");
        for(size_t i = 0; i < num_points; ++i) {
            points.emplace_back(dist(rd), dist(rd));
        }
    }
    if (points.size() < 100)
        std::cout << points << std::endl;
    // median_le(points.begin(), points.end(), 0);
    // std::cout << points << std::endl;
    // median_le(points.begin(), points.end(), 1);
    // std::cout << points << std::endl;
    //std::vector<std::reference_wrapper<point>> idxx(points.begin(), points.end());
    //std::vector<std::reference_wrapper<point>> idxy(points.begin(), points.end());
    kdtree t;
    {
        TimeMeasure timer(true, "building KD tree");
        t.build(points);
    }
    if (points.size() < 100)
        t.print();

    for(double c = (num_points / 3.0) / 2.0; c < num_points; c += num_points / 3.0) {
        TimeMeasure timer(true, "Searching point");
        point ref(c, c);
        std::cout << "Searching for nearest neighbour for " << ref << std::endl;
        auto result = t.nearest_neighbour(ref);
        std::cout << result
                  << std::endl;
    }

    std::cout << "Searching for " << num_points << " random points in tree:" << std::endl;
    point_vector testpoints;
    {
        TimeMeasure timer(true, "generating random test points");
        for(size_t i = 0; i < num_points; ++i) {
            testpoints.emplace_back(dist(rd), dist(rd));
        }
    }
    {
        TimeMeasure timer(true, "Searching point series");
        for(const auto& e : testpoints) {
            auto result = t.nearest_neighbour(e);
        }
    }

    {
        // test from rosetta code
        // http://www.rosettacode.org/wiki/K-d_tree
        std::cout << "Rosetta code test (http://www.rosettacode.org/wiki/K-d_tree)" << std::endl;
        point rctest[] {{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}};
        kdtree rct;
        rct.build(std::begin(rctest), std::end(rctest));
        rct.print();
        point rcp { 9, 2 };
        auto rcresult = rct.nearest_neighbour(rcp);
        std::cout << "Expected: (8, 1): " 
                  << rcresult
                  << std::endl;
    }
    return 0;
}