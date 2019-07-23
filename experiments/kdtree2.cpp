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
        explicit search_def(const point& p, size_t max_results = 1
                          , double max_distance = std::numeric_limits<double>::infinity()) 
        : ref_{p}, max_results_{max_results}, max_distance_norm_ { max_distance * max_distance }
        {}
        search_def(const search_def&) = default;
        search_def& operator=(const search_def&) = default;
        //double distance() const noexcept { return std::sqrt(distance_norm_); }
        const point ref_;  // point to search for
        //const point* result_ { nullptr };  // closest match found
        using result_type = std::pair<point const*, double>;
        std::vector<result_type> results_;
        static bool cmp(const result_type& a, const result_type& b) {
            return a.second < b.second;
        };
        size_t max_results_ { 1 }; // return nearest max_results points
        double max_distance_norm_ ;     // return all points up to this distance
        bool is_full() const noexcept { return results_.size() == max_results_; }
        double max_best_distance_norm() const noexcept {
            if (results_.size() == 0)
                return std::numeric_limits<double>::infinity();
            return results_[0].second; 
        }
        // bool consider(double distance_norm) const noexcept {
        //     return ( is_full() && distance_norm < max_best_distance_norm())
        //         || (!is_full() && distance_norm <= max_distance_norm_); 
        // }
        enum class CONSIDER { NOT, REPLACE, APPEND };
        search_def::CONSIDER consider_check(double distance_norm) const noexcept {
            if (is_full() && distance_norm < max_best_distance_norm())
                return search_def::CONSIDER::REPLACE;
            if (!is_full() && distance_norm <= max_distance_norm_)
                return search_def::CONSIDER::APPEND;
            return search_def::CONSIDER::NOT;   
        }
        void check_insert_result(point const * p, double distance_norm) {
            auto consider = consider_check(distance_norm);
            switch(consider) {
                case search_def::CONSIDER::REPLACE:
                    std::pop_heap(results_.begin(), results_.end(), cmp);
                    results_.back() = std::make_pair(p, distance_norm);
                    std::push_heap(results_.begin(), results_.end(), cmp);
                    break;
                case search_def::CONSIDER::APPEND:
                    results_.push_back(std::make_pair(p, distance_norm));
                    std::push_heap(results_.begin(), results_.end(), cmp);
                    break;
                case search_def::CONSIDER::NOT:
                    // no action
                    // fallthrough
                default:
                    // no action
                ;
            }
            // // if (is_full() && distance_norm < max_best_distance_norm()) {
            //     std::pop_heap(results_.begin(), results_.end(), cmp);
            //     results_.back() = std::make_pair(p, distance_norm);
            //     std::push_heap(results_.begin(), results_.end(), cmp);
            // } else if (!is_full() && distance_norm <= max_distance_norm_) {
            //     results_.push_back(std::make_pair(p, distance_norm));
            //     std::push_heap(results_.begin(), results_.end(), cmp);
            // }
        }
        void sort_results() {
            std::sort_heap(results_.begin(), results_.end(), cmp);
        }
        //double distance_norm_ { std::numeric_limits<double>::infinity() };  // square! of distance
        unsigned nodes_visited_ { 0 };  // number of nodes visited during search
        friend std::ostream& operator<<(std::ostream& o, const search_def& sd) {
            o << "Reference point: " << sd.ref_
              << " max results: " << sd.max_results_ 
              << " max distance: " << std::sqrt(sd.max_distance_norm_ )
              << " Found: " << std::endl;
            for(size_t i = 0; i < sd.results_.size(); ++i) {
                result_type const& r = sd.results_[i];
                o << (i+1) << ": "; 
                if (r.first == nullptr)
                    o << "(none)";
                else 
                    o << *r.first;
                o << " Distance " << std::sqrt(r.second)
                  << std::endl;
            }
            o << " nodes visited: " << sd.nodes_visited_;
            return o;
        }
    };
    search_def nearest_neighbours(const point& ref, size_t max_results = 1
                                , double max_distance = std::numeric_limits<double>::max()) {
        search_def search (ref, max_results, max_distance);
        if ( root_ == NOCHILD )
            return search;
        find_nearest_neighbours(search, root_, 0);
        search.sort_results();
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
    void find_nearest_neighbours(search_def& search, size_t node_idx, unsigned depth) {
        if (node_idx == NOCHILD)
            return;
        ++search.nodes_visited_;
        kdtree::kdnode& n = t_[node_idx];
        auto i = depth % 2;
        auto d = search.ref_.distance_norm(n.p_);
        search.check_insert_result(&n.p_, d);
        double plane_dist  = n.p_[i] - search.ref_[i];
        double plane_dist2 = plane_dist * plane_dist;
        // current splitting hyperplane is right of ref point -> search left subtree
        // otherwise if hyperplane is left of ref point -> search right subtree
        auto next_node = (plane_dist > 0) ? n.left_ : n.right_;
        find_nearest_neighbours(search, next_node, depth + 1);
        // if best distance is less equal the distance of the hyperplane, we
        // don't need to check other side
        if ( search.consider_check(plane_dist2) == search_def::CONSIDER::NOT )
            return;
        // otherwise search other side as well
        next_node = (plane_dist > 0) ? n.right_ : n.left_;
        find_nearest_neighbours(search, next_node, depth + 1);
    }

};



int main(int argc, char const *argv[])
{
    using namespace std;

    size_t num_points = 10;
    if (argc > 1)
        num_points = std::stoul(argv[1]);
    size_t num_results = 1;
    if (argc > 2)
        num_results = std::stoul(argv[2]);
    double max_dist = num_points;
    if (argc > 3)
        max_dist = std::stod(argv[3]);
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
        auto result = t.nearest_neighbours(ref, num_results, max_dist);
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
            auto result = t.nearest_neighbours(e, num_results, max_dist);
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
        auto rcresult = rct.nearest_neighbours(rcp, num_results, max_dist);
        std::cout << "Expected: (8, 1): " 
                  << rcresult
                  << std::endl;
    }
    return 0;
}