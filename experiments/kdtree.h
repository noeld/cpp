// http://www.rosettacode.org/wiki/K-d_tree

// TODO:
// [x] add nearest_neighbours search
// [x] add max_results search parameter
// [x] add max_distance search parameter
// [ ] add support for dimensions > 2
// [ ] add meta information to point (payload)
// [ ] cleanup, remove old code
// [ ] proper documentation (doxygen?)
// [ ] add proper command line options (boost)
// [ ] add README.md
// [x] add unit tests (boost)
// [x] split into header-only kd-tree and test-cases / sample app

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <limits>
#include "timemeasure.h"

struct point {
private:
    int x_ {0};
    int y_ {0};
public:
    point() = default;
    point(int x, int y) : x_{x}, y_{y} {}
    point(const point&) = default;
    point& operator=(const point&) = default;

    // accessors for the first n dimensions
    int  x() const noexcept { return x_; }
    int& x() noexcept       { return x_; }
    int  y() const noexcept { return y_; }
    int& y() noexcept       { return y_; }
    // int  z() const noexcept { return z_; }
    // int& z() noexcept       { return z_; }

    // 
    int operator[](size_t i) const noexcept {
        return (i == 0) ? x_ : y_;
    }
    int& operator[](size_t i) noexcept {
        return (i == 0) ? x_ : y_;   
    }

    friend bool operator==(const point& a, const point& b) {
        return a.x_ == b.x_ && a.y_ == b.y_;
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
    auto acc = [dim](const typename T::value_type& a, const typename T::value_type& b) -> bool { return a[dim] < b[dim]; };
    std::nth_element(start, m, end, acc);
    for(auto m2 = m + 1; m2 != end && (*m2)[dim] == (*m)[dim]; ++m2, ++m);
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
        const point ref_;  // point to search for ( reference point )
        struct result_type {
            result_type() = default;
            result_type(point const* p, const double& distance_norm) : p_{p}, distance_norm_{distance_norm}
            {}
            point const* p_ { nullptr };               // const pointer to point
            double distance_norm_ { std::numeric_limits<double>::max() };         // distance norm
            double distance() const noexcept { return std::sqrt(distance_norm_);}
        };
        std::vector<result_type> results_; // result vector
        static bool cmp(const result_type& a, const result_type& b) {
            return a.distance_norm_ < b.distance_norm_;
        };
        size_t max_results_ { 1 };  // return nearest max_results points
        double max_distance_norm_;  // return all points up to this distance
        bool is_full() const noexcept { return results_.size() == max_results_; }
        double max_best_distance_norm() const noexcept {
            if (results_.size() == 0)
                return std::numeric_limits<double>::infinity();
            return results_[0].distance_norm_; 
        }
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
                    results_.back() = { p, distance_norm };
                    std::push_heap(results_.begin(), results_.end(), cmp);
                    break;
                case search_def::CONSIDER::APPEND:
                    results_.emplace_back(p, distance_norm);
                    std::push_heap(results_.begin(), results_.end(), cmp);
                    break;
                case search_def::CONSIDER::NOT:
                    // no action
                    // fallthrough
                default:
                    // no action
                ;
            }
        }
        void sort_results() {
            std::sort_heap(results_.begin(), results_.end(), cmp);
        }
        unsigned nodes_visited_ { 0 };  // number of nodes visited during search
        friend std::ostream& operator<<(std::ostream& o, const search_def& sd) {
            o << "Reference point: " << sd.ref_
              << " max results: " << sd.max_results_ 
              << " max distance: " << std::sqrt(sd.max_distance_norm_ )
              << " Found: " << std::endl;
            for(size_t i = 0; i < sd.results_.size(); ++i) {
                result_type const& r = sd.results_[i];
                o << (i+1) << ": "; 
                if (r.p_ == nullptr)
                    o << "(none)";
                else 
                    o << *r.p_;
                o << " Distance " << r.distance()
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


