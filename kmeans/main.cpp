#include<iostream>
#include<random>
#include<tuple>
#include<utility>
#include<cmath>
#include<vector>
#include<thread>
#include<chrono>
#include<string>
#include<algorithm>
#include<codecvt>
#include<locale>

// for VS builds
#undef max
#include <taskflow/taskflow.hpp>

#include<boost/format.hpp>

#include "point.h"
#include "group.h"
#include "draw.h"
#include "timer.h"
#include "options.h"

#undef max

template<typename T = elt_type>
T uniform_rand(T const& max = T(1.0))
{
    static std::default_random_engine rnd;
    using distr_type = std::conditional<std::is_floating_point<T>::value, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>::type;
    static distr_type dist;
    dist.param(distr_type::param_type{ T(0.0), max });
    return dist(rnd);
}

/// Generate some random points
struct generator
{
    // std::random_device rd_;
    std::default_random_engine rd_;
    std::uniform_real_distribution<elt_type> dist_r_{ 0.0, 1.0 }; 
    std::uniform_real_distribution<elt_type> dist_a_ {0.0, 2.0 * M_PI};
    std::uniform_int_distribution<size_t> dist_p_;
    runtime_options const & opt_;

    explicit generator(runtime_options const & opt) 
        : opt_{ opt }
        , dist_r_ { 0.0, opt.generator_points_radius }
    {
        rd_.seed(std::chrono::system_clock::now().time_since_epoch().count());
        dist_p_.param(std::uniform_int_distribution<size_t>::param_type(1, opt_.generator_centers_count));
        centers_ = (opt_.generator_centers_randomize) ? dist_p_(rd_) : opt_.generator_centers_count;
        dist_p_.param(std::uniform_int_distribution<size_t>::param_type(1, centers_));
        float df = opt.generator_centers_radius;
        for(size_t n = 0; n < centers_; ++n)
            center_points_.push_back(random_point() * df);
    }

    point_type random_point() 
    {
        elt_type ang = dist_a_(rd_);
        elt_type r   = dist_r_(rd_);
        return {r * std::cos(ang), r * std::sin(ang)};
    }

    /// Generate random points around a random number of random centers
    point_type random_center_point()
    {
        auto p = random_point();
        auto ind = dist_p_(rd_);
        p += center_points_[ind];
        return p;
    }
    size_t centers_;
    std::vector<point_type> center_points_;
};


/*
void kmeans(unsigned clusters, std::vector<point_type> const & points
           , std::vector<group_info> & groups
           , std::vector<center_info> & centers)
{
    centers.resize(clusters);
    
    for(size_t i = 0; i < groups.size(); ++i)
        groups[i].group_ = i % clusters;
    auto nearest = [&centers](point_type const & p)
    {
        auto dist = std::numeric_limits<elt_type>::max();
        int c = 0;
        size_t i = 0, min_i = 0;
        for(auto const & c : centers)
        {
            auto d = distance(p, c.point_);
            if (d < dist)
            {
                min_i = i;
                dist = d;
            }
            ++i;
        }
        return min_i;
    };
    size_t changed_points = 0;
    do {
        for(auto & c : centers)
        {
            c.point_ = {0, 0};
            c.count_ = 0;
        }
        for(size_t i = 0; i < points.size(); ++i)
        {
            center_info& c = centers[groups[i].group_];
            c.count_++;
            point_type const & p = points[i];
            c.point_ += p;
        }
        for(auto & c : centers)
            c.point_ /= c.count_ == 0 ? 1 : c.count_;
        
        changed_points = 0;
        for(size_t i = 0; i < points.size(); ++i)
        {
            point_type const & p = points[i];
            group_info& g = groups[i];
            auto gi = nearest(p);
            if (gi != g.group_)
            {
                ++changed_points;
                g.group_ = gi;
            }
        }
        draw2(points, groups, &centers);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (changed_points > points.size() * 0.01);
}
*/

std::tuple<size_t, elt_type> nearest(point_type const& p, std::vector<center_info>& centers, size_t n = 0)
{
    if (n == 0)
        n = centers.size();
    auto dist = std::numeric_limits<elt_type>::max();
    int c = 0;
    size_t i = 0, min_i = 0;
//    for (auto const& c : centers)
    for (size_t i = 0; i < n; ++i)
    {
        auto const& c = centers[i];
        auto d = distance2(p, c.point_);
        if (d < dist)
        {
            min_i = i;
            dist = d;
        }
    }
    return std::make_tuple(min_i, dist);
};

void kpp(std::vector<point_type> const& points
    , std::vector<group_info>& groups
    , std::vector<center_info>& centers)
{
    centers[0].point_ = points[uniform_rand(points.size())];
    std::vector<elt_type> distances(points.size());
    elt_type sum;
    for (size_t n = 1; n < centers.size(); ++n)
    {
        sum = elt_type(0.0);
        for (size_t i = 0; i < points.size(); ++i)
        {
            auto const& p = points[i];
            auto [idx, dist] = nearest(p, centers, n);
            sum += dist;
            distances[i] = dist;
        }
        sum = uniform_rand(sum);
        for (size_t i = 0; i < points.size(); ++i)
        {
            sum -= distances[i];
            if (sum > 0)
                continue;
            centers[n].point_ = points[i];
            break;
        }
    }
    for (size_t i = 0; i < points.size(); ++i)
    {
        auto [idx, dist] = nearest(points[i], centers);
        groups[i].group_ = idx;
    }
}

void kmeansp(unsigned clusters, std::vector<point_type> const & points
           , std::vector<group_info> & groups
           , std::vector<center_info> & centers
           , runtime_options const & opt)
{
    bounding_rect bounds(points);
    centers.resize(clusters);
    
    if (opt.kmeans_kpp)
    {
        // initialize with kpp
        kpp(points, groups, centers);
    }
    else {
        // randomly initialize cluster centers
        for (size_t i = 0; i < groups.size(); ++i)
            groups[i].group_ = i % clusters;
    }

    size_t changed_points = 0;
    tf::Executor executor;
    tf::Taskflow taskflow;
    if (opt.kmeans_draw)
    {
        draw2(points, groups, opt, &centers, &executor, &bounds);
        std::this_thread::sleep_for(std::chrono::milliseconds(opt.kmeans_sleep_ms));
    }
    do {
        for(auto & c : centers)
        {
            c.point_ = {0, 0};
            c.count_ = 0;
        }
        // taskflow.transform_reduce(I beg, I end, T &result, B &&bop, P &&pop, U &&uop)
        for(size_t i = 0; i < points.size(); ++i)
        {
            center_info& c = centers[groups[i].group_];
            c.count_++;
            point_type const & p = points[i];
            c.point_ += p;
        }
        for (auto& c : centers)
        {
            if (c.count_ > 0)
                c.point_ /= c.count_;
        }
        
        changed_points = 0;
        // for(size_t i = 0; i < points.size(); ++i)
        taskflow.parallel_for((int)0, (int)points.size(), 1,
            [&](int i) {
            point_type const & p = points[i];
            group_info& g = groups[i];
            auto [gi, dist] = nearest(p, centers);
            if (gi != g.group_)
            {
                ++changed_points;
                g.group_ = gi;
            }
        },opt.kmeans_parallel_partitions);
        executor.run(taskflow).get();
        if (opt.kmeans_draw)
        {
            draw2(points, groups, opt, &centers, &executor, &bounds);
            std::this_thread::sleep_for(std::chrono::milliseconds(opt.kmeans_sleep_ms));
        }
    } while (changed_points > points.size() * opt.kmeans_stop_fct);
}

int main(int argc, char const *argv[])
{
    int ret = 0;

    runtime_options rto;
    auto res = get_options(argc, argv, rto);
    if (res != 0)
        exit(1);

    GDIPlus gdiplus;

    std::vector<point_type> points;

    timer generate_timer;
    if (rto.load_filename.size() > 0)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wfilename = converter.from_bytes(rto.load_filename);
        points.resize(0);
        points_from_image(wfilename, points);
    }
    else {
        generator gen(rto);
        points.resize(rto.generator_points_count);
        for (auto& e : points)
            e = gen.random_center_point();
    }
    
    if (rto.generator_points_sort)
    {
        std::sort(points.begin(), points.end()
            , [](point_type const& a, point_type const& b) -> bool
            { return std::tie(a[0], a[1]) < std::tie(b[0], b[1]); }
        );
    }

    generate_timer.stop();

    std::vector<group_info> groups(points.size());

    timer compute_timer;
    bounding_rect br(points);
    std::cout << "bounding rect: " << br << std::endl;
    draw2(points, groups, rto, nullptr, nullptr, &br);
    
    std::vector<center_info> centers(rto.kmeans_clusters);
    kmeansp(rto.kmeans_clusters, points, groups, centers, rto);
    compute_timer.stop();
    draw2(points, groups, rto, &centers, nullptr, &br);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    boost::format fmt{"generate: %6.3fs - compute: %6.3fs"};
    std::cout << (fmt % generate_timer.seconds_last_stop() % compute_timer.seconds_last_stop()) << std::endl;
    for (auto& e : centers)
        std::cout << e.point_ << std::endl;
    return ret;
}