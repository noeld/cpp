#pragma once

#include "point.h"
#include "group.h"
#include <vector>
#include <windows.h>
#include <Gdiplus.h>
#include <taskflow/taskflow.hpp>

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
	     , std::vector<center_info> * centers = nullptr
	     , tf::Executor* executor = nullptr);

/**
 * Retrieve point from an image:
 * load image as greyscale
 * return (hue, luminance)
 */
void points_from_image(std::wstring const & file_name
	                , std::vector<point_type> & points);