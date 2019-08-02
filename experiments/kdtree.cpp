// http://www.rosettacode.org/wiki/K-d_tree

#include "kdtree.h"
#include "kdrandom.h"
#include <string>

using kdtree_type = kdtree<int, 2, std::string>;
using point_type = kdtree_type::point_type;
using point_vector = std::vector<point_type>;

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


int main(int argc, char const *argv[])
{
    using namespace std;

    size_t num_points = 10; // number of sample points
    if (argc > 1)
        num_points = std::stoul(argv[1]);
    size_t num_results = 1; // number of results
    if (argc > 2)
        num_results = std::stoul(argv[2]);
    double max_dist = num_points; // maximum distance
    if (argc > 3)
        max_dist = std::stod(argv[3]);
    // random_generator<point_type> rnd(num_points);
    // point_vector points(num_points);
    // {
    //     TimeMeasure timer(true, "generating random points");
    //     rnd.for_each(points.begin(), points.end());
    // }
    // if (points.size() < 100)
    //     std::cout << points << std::endl;
    // kdtree_type t;
    // {
    //     TimeMeasure timer(true, "building KD tree");
    //     t.build(points);
    // }
    // if (points.size() < 100)
    //     t.print();

    // for(double c = (num_points / 3.0) / 2.0; c < num_points; c += num_points / 3.0) {
    //     TimeMeasure timer(true, "Searching point");
    //     kdtree_type::value_type cc = static_cast<kdtree_type::value_type>(c);
    //     point_type ref{cc, cc}; 
    //     std::cout << "Searching for nearest neighbour for " << ref << std::endl;
    //     auto result = t.nearest_neighbours(ref, num_results, max_dist);
    //     std::cout << result
    //               << std::endl;
    // }

    // std::cout << "Searching for " << num_points << " random points in tree:" << std::endl;
    // point_vector testpoints(num_points);
    // {
    //     TimeMeasure timer(true, "generating random test points");
    //     rnd.for_each(testpoints.begin(), testpoints.end());
    // }
    // {
    //     TimeMeasure timer(true, "Searching point series");
    //     for(const auto& e : testpoints) {
    //         auto result = t.nearest_neighbours(e, num_results, max_dist);
    //     }
    // }

    kdtree_type rct;
    {
        // test from rosetta code
        // http://www.rosettacode.org/wiki/K-d_tree
        std::cout << "Rosetta code test (http://www.rosettacode.org/wiki/K-d_tree)" << std::endl;
        point_type rctest[] {{"1", {2, 3}}, {"2", {5, 4}}, {"3", {9, 6}}, {"4", {4, 7}}, {"der hier", {8, 1}}, {"6", {7, 2}}};
        rct.build(std::begin(rctest), std::end(rctest));
        rct.print();
        point_type rcp { 9, 2 };
        auto rcresult = rct.nearest_neighbours(rcp, num_results, max_dist);
        std::cout << "Expected: (8, 1): " 
                  << rcresult
                  << std::endl;
    }
    {

        // test contains
        kdtree tt;
        std::vector<kdtree<>::point_type> test(num_points);
        auto const range = num_points * 10;
        auto const extend = num_points / 10;
        random_generator<kdtree<>::point_type> rg(range);
        for(auto& e : test) {
            rg(e);
            for(size_t i = 0; i < kdtree_type::contained_point_type::size(); ++i) {
                auto ii = i + kdtree_type::contained_point_type::size();
                if (e[i] > e[ii])
                    std::swap(e[i], e[ii]);
                if (e[ii] - e[i] > extend)
                    e[ii] = e[i] + extend;
            }
        }
        tt.build(std::begin(test), std::end(test));
        if (num_points <= 10)
            tt.print();
        for (int n = 0; n <= range; n += range / 100) {
            kdtree_type::contained_point_type p{n};
            std::cout << "Checking " << p << std::endl;
            auto res = tt.containing_hyperrects(p, std::numeric_limits<size_t>::max());
            for(auto & e : res) {
                std::cout << *e << std::endl;
            }
        }
    }
    return 0;
}