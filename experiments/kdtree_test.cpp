#define BOOST_TEST_MODULE KDTree2Tests
#include <boost/test/included/unit_test.hpp>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "kdtree.h"

// template<typename T>
// std::string tos(const T& s) {
//     std::ostringstream ss;
//     ss << s;
//     return ss.str();
// }

point rctest[] {{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}};

struct KDFixture {
    KDFixture() {
        t.build(std::begin(rctest), std::end(rctest));
    }
    kdtree t;    
    const point rc_expected {8, 1};
    const point rc_reference { 9, 2 };
};

BOOST_FIXTURE_TEST_SUITE(KDSuitem, KDFixture) 
    
    BOOST_AUTO_TEST_CASE(EqualTEst)
    {
        point a { 5, 4 };
        point b { 5, 4 };
        point c { 0, 4 };
        point d { 5, 0 };
        point e { 1, 1 };
        BOOST_CHECK(a == a);
        BOOST_CHECK(a == b);
        BOOST_CHECK(! (a == c));
        BOOST_CHECK(! (a == d));
        BOOST_CHECK(! (a == e));
    }

    BOOST_AUTO_TEST_CASE(RosettaCode)
    {
        auto r = t.nearest_neighbours(rc_reference);
        BOOST_TEST(r.results_.size() == 1);
        auto const& r0 = r.results_[0];
        BOOST_TEST(*r0.p_ == rc_expected);
    }

    BOOST_AUTO_TEST_CASE(RC_max_params)
    {
        // 1: (8; 1) Distance 1.41421
        // 2: (7; 2) Distance 2
        // 3: (9; 6) Distance 4
        // 4: (5; 4) Distance 4.47214
        // 5: (4; 7) Distance 7.07107
        // 6: (2; 3) Distance 7.07107
        //  nodes visited: 6
        auto r1 = t.nearest_neighbours(rc_reference);
        BOOST_CHECK_EQUAL(1, r1.results_.size());

        auto r2 = t.nearest_neighbours(rc_reference, 4);
        // BOOST_TEST_MESSAGE("Result: " << r2 );
        BOOST_CHECK_EQUAL(4, r2.results_.size());
        
        auto r3 = t.nearest_neighbours(rc_reference, 4, 5.0);
        // BOOST_TEST_MESSAGE("Result: " << r3 );
        BOOST_CHECK_EQUAL(4, r3.results_.size());
        auto m = std::max_element(std::begin(r3.results_), std::end(r3.results_), kdtree::search_def::cmp);
        // BOOST_TEST_MESSAGE ( "Max " << m->distance() );
        BOOST_CHECK(m->distance() <= 5.0);

        auto r4 = t.nearest_neighbours(rc_reference, 4, 4.0);
        // BOOST_TEST_MESSAGE("Result: " << r4 );
        BOOST_CHECK_EQUAL(3, r4.results_.size());
        auto m4 = std::max_element(std::begin(r4.results_), std::end(r4.results_), kdtree::search_def::cmp);
        // BOOST_TEST_MESSAGE ( "Max " << *m4->p_ );
        // BOOST_TEST_MESSAGE ( "Max " << m4->distance() );
        BOOST_CHECK(m4->distance() <= 4.0);
           
    }

BOOST_AUTO_TEST_SUITE_END()


