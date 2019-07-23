// http://www.rosettacode.org/wiki/K-d_tree

#include "kdtree.h"

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