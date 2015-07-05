#pragma once

#include <string>
using std::string;

class PicData {
public:
	bool write(const string& filename) const;
	bool read(const string& filename);
	unsigned iter(const double& cx, const double& cy) const;
public:
	unsigned	max_it		{2500};
	double		max_betrag	{10.0};
	unsigned	width		{500};
	unsigned	height		{500};
	double		vx			{-0.7202};
	double		vy			{-0.082500125};
	double		distance	{1.0};
	double		rel			{1.0}; // h/w
	unsigned	max_frames	{10};
	unsigned	frame		{1};
	double		dist_factor	{0.9};
};
//const picdata_t stddata = {2500, 10.0, 500, 500, -0.7202, -0.082500125, 1.0, 1.0, 10, 1, 0.9};


