#include<iostream>
#include<random>
#include<tuple>
#include<utility>
#include<cmath>
#include<vector>
#include<thread>
#include<chrono>

#include "point.h"
#include "group.h"
#include "draw.h"



struct generator
{
	std::random_device rd_;
	std::uniform_real_distribution<elt_type> dist_r_{ 0.0, 1.0 };
	std::uniform_real_distribution<elt_type> dist_a_ {0.0, 2.0 * M_PI};

	point_type random_point() 
	{
		elt_type ang = dist_a_(rd_);
		elt_type r   = dist_r_(rd_);
		return {r * std::cos(ang), r * std::sin(ang)};
	}
};

struct center_info
{
	point_type point_;
	size_t count_ {0};
};

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
			c.point_ /= c.count_;
		
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
		draw2(points, groups);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	} while (changed_points > points.size() * 0.01);
}

int main(int argc, char const *argv[])
{
	int ret = 0;
	size_t n = 100;
	int clusters = 3;
	if (argc > 1)
	{
		n = std::atoll(argv[1]);
	}
	if (argc > 2)
	{
		clusters = std::atoll(argv[2]);
	}
	GDIPlus gdiplus;
	generator gen;
	std::vector<center_info> centers;
	// std::vector<point_type> points(n);
	std::vector<point_type> points;
	
	// auto file_name = L"C:\\Users\\Arnold\\Pictures\\down_wallpaper_f800r_02_1280x1024.jpg";
	// auto file_name = L"C:\\Users\\Arnold\\Pictures\\Passfotos3.png";
	auto file_name = L"C:\\Users\\Arnold\\Pictures\\_MG_1166.jpg";
	
	points_from_image(file_name, points);
	// for(auto & e : points)
	// 	e = gen.random_point();

	std::vector<group_info> groups(points.size());

	draw2(points, groups);

	kmeans(clusters, points, groups, centers);

	draw2(points, groups);

	std::this_thread::sleep_for(std::chrono::seconds(5));
	return ret;
}