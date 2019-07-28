#pragma once
// http://www.rosettacode.org/wiki/K-d_tree

// TODO:
// [x] add nearest_neighbours search
// [x] add max_results search parameter
// [x] add max_distance search parameter
// [x] add support for dimensions > 2
// [ ] add support for vectorization
// [x] add support for other dimensions types than int
// [ ] add meta information to point (payload)
// [x] cleanup, remove old code
// [ ] add ability to add and remove points, rebuild tree
// [ ] proper documentation (doxygen?)
// [ ] add proper command line options (boost)
// [ ] add namespace
// [ ] add README.md
// [x] add unit tests (boost)
// [x] split into header-only kd-tree and test-cases / sample app
// [x] add find_containing_hyper_rect (point<T, n> IN point<T, n * 2>)
// [ ] add find-strategies interface

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <limits>
#include <initializer_list>
#include "timemeasure.h"

template<typename DIM_TYPE = int, size_t DIMENSIONS = 2>
struct point {
    static constexpr size_t size() noexcept { return DIMENSIONS; };
    using value_type = DIM_TYPE;
    using element_type = std::array<value_type, DIMENSIONS>;
private:
    element_type e_;
public:
    point() = default;
    point(const std::initializer_list<value_type>& list) {
        std::copy_n(std::begin(list), DIMENSIONS, std::begin(e_));
    }
    // template<typename Ts ...>
    // point(Ts&& ... ts) : e_ { std::forward<Ts>(ts)... } {}

    point(const point&) = default;
    point& operator=(const point&) = default;

    value_type operator[](size_t i) const noexcept {
        return e_[i];
    }
    value_type& operator[](size_t i) noexcept {
        return e_[i];
    }

    friend bool operator==(const point& a, const point& b) {
        return a.e_ == b.e_;
    }
    friend bool operator!=(const point& a, const point& b) {
        return !(a == b);
    }
    point operator-(const point& b) {
        point tmp;
        for(size_t i = 0; i < size(); ++i) {
            tmp[i] = this->operator[](i) - b[i];
        }
        return tmp;
    }
    double distance_norm(const point& other) const {
        double res = 0.0;
        for(size_t i = 0; i < DIMENSIONS; ++i) {
            double dd = static_cast<double>(operator[](i) - other[i]);
            res += dd * dd;
        }
        return res;
    }
    friend std::ostream& operator<<(std::ostream& o, const point& p) {
        o << '(' << p[0];
        for(size_t i = 1; i < DIMENSIONS; ++i) o << "; " << p[i];
        return o << ')';
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

template<typename DIM_TYPE = int, size_t DIMENSIONS = 2>
struct kdtree {
    static const size_t NOCHILD = std::numeric_limits<size_t>::max();
    static constexpr size_t dimensions() noexcept { return DIMENSIONS; }
    using value_type = DIM_TYPE;
    using point_type = point<value_type, DIMENSIONS>;

    struct kdnode {
        point_type p_;
        value_type operator[](size_t i) const noexcept {
            return p_[i];
        }
        size_t left_ { NOCHILD }, right_ {NOCHILD};
        kdnode& operator=(const point_type& p) {
            p_ = p;
        }
        unsigned depth_ { 0 };
        unsigned dim() const noexcept { return depth_ % DIMENSIONS; }
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
        explicit search_def(const kdtree::point_type& p, size_t max_results = 1
                          , double max_distance = std::numeric_limits<double>::infinity()) 
        : ref_{p}, max_results_{max_results}, max_distance_norm_ { max_distance * max_distance }
        {}

        search_def(const search_def&) = default;

        search_def& operator=(const search_def&) = default;

        const kdtree::point_type ref_;  // point to search for ( reference point )

        struct result_type {
            result_type() = default;
            result_type(kdtree::point_type const* p, const double& distance_norm) 
            : p_{p}, distance_norm_{distance_norm}
            {}
            kdtree::point_type const* p_ { nullptr };               // const pointer to point
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

        void check_insert_result(kdtree::point_type const * p, double distance_norm) {
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

    search_def nearest_neighbours(const point_type& ref, size_t max_results = 1
                                , double max_distance = std::numeric_limits<double>::max()) 
    {
        search_def search (ref, max_results, max_distance);
        if ( root_ == NOCHILD )
            return search;
        find_nearest_neighbours(search, root_, 0);
        search.sort_results();
        return search;
    }

    using contained_point_type = point<typename point_type::value_type, point_type::size() / 2>;
    std::vector<point_type const *> containing_hyperrects(const contained_point_type& ref, size_t max_results = 1) 
    {
        std::vector<point_type const *> result;
        // search_def search (ref, max_results, std::numeric_limits<double>::max());
        // auto contained = [ref](const point_type& p, size_t dim) -> int {
        //     auto d = 
        // }
        unsigned nodes_visited { 0 };
        unsigned depth { 0 };
        struct status_type { size_t node_idx_; unsigned depth_; };
        std::vector<status_type> stack;
        stack.push_back({root_, 0u});
        while(stack.size() > 0) {
            auto const& ctx = stack.back();
            stack.pop_back();
            if (ctx.node_idx_ != NOCHILD)
            {
                ++nodes_visited;
                kdtree::kdnode& current_node = t_[ctx.node_idx_]; 
                bool contained { true };
                for(size_t i = 0; i < DIMENSIONS / 2; ++i) {
                    contained = 
                    contained && ref[i] >= current_node.p_[i] 
                              && ref[i] <= current_node.p_[i + contained_point_type::size()]
                    ;
                }
                if (contained) {
                    result.push_back(&current_node.p_);
                    if (result.size() >= max_results)
                        break;
                    stack.push_back({current_node.right_, ctx.depth_ + 1});
                    stack.push_back({current_node.left_, ctx.depth_ + 1});
                } else {
                    auto dim = ctx.depth_ % DIMENSIONS;
                    auto dim2 = ctx.depth_ % contained_point_type::size();
                    auto dist = current_node.p_[dim] - ref[dim2];
                    if (dim == dim2) {
                        // looking at the lower bound dims
                        stack.push_back({current_node.left_, ctx.depth_ + 1});
                        if (dist <= 0) {
                            // current nodes lower bound is left of ref
                            // cannot skip right subtree
                            stack.push_back({current_node.right_, ctx.depth_ + 1});
                        }
                    } else {
                        // looking at the upper bound dims
                        stack.push_back({current_node.right_, ctx.depth_ + 1});
                        if (dist > 0) {
                            // current nodes upper bound is right of ref
                            // cannot skip left subtree
                            stack.push_back({current_node.left_, ctx.depth_ + 1});
                        }
                    }
                }
            }
        }
        std::cout << "Nodes visited: " << nodes_visited << std::endl;
        return result;
    }

private:
    std::vector<kdtree::kdnode> t_;
    size_t root_ { NOCHILD };

protected:
    size_t buildtree(decltype(t_.begin()) begin, decltype(t_.begin()) end, unsigned depth) {
        if (begin == end)
            return NOCHILD;
        auto m = median_le(begin, end, depth % DIMENSIONS);
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
        auto i = depth % DIMENSIONS;
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


