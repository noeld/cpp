#pragma once

#include "point.h"
#include "group.h"
#include "options.h"
#undef max
#include <taskflow/taskflow.hpp>
#include <vector>
#include <windows.h>
#include <gdiplus.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif // !M_PI


struct  GDIPlus
{
	GDIPlus();

	~GDIPlus();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
};

// void draw(std::vector<point_type> const & points
// 	     ,std::vector<group_info> const & groups);

void draw2(std::vector<point_type> const & points
	     , std::vector<group_info> const & groups
	     , runtime_options const & opt
	     , std::vector<center_info> * centers = nullptr
	     , tf::Executor* executor = nullptr
		 , bounding_rect const * bounds = nullptr
);

/**
 * Retrieve point from an image:
 * load image as greyscale
 * return (hue, luminance)
 */
void points_from_image(std::wstring const & file_name
	                , std::vector<point_type> & points);