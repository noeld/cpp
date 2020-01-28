#pragma once

#include "point.h"
#include "group.h"
#include <vector>
#include <windows.h>
#include <Gdiplus.h>


struct  GDIPlus
{
	GDIPlus();

	~GDIPlus();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
};

void draw(std::vector<point_type> const & points
	     ,std::vector<group_info> const & groups);

void draw2(std::vector<point_type> const & points
	     ,std::vector<group_info> const & groups);