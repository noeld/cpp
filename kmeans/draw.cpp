// http://supercomputingblog.com/graphics/getting-started-with-gdi-in-visual-studio/

#include "draw.h"
#include "group.h"
#include <iostream>
#include <limits>

#undef max
#undef min

GDIPlus::GDIPlus()
{
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

GDIPlus::~GDIPlus()
{
  Gdiplus::GdiplusShutdown(gdiplusToken);   
}


const COLORREF group_cols[] = {
  RGB(0x7f, 0x7f, 0x7f)
  , RGB(0xff, 0x00, 0x00)
  , RGB(0x00, 0xff, 0x00)
  , RGB(0x00, 0x00, 0xff)
  , RGB(0xff, 0xff, 0x00)
  , RGB(0x00, 0xff, 0xff)
  , RGB(0xff, 0x00, 0xff)
  , RGB(0xff, 0x7f, 0x7f)
  , RGB(0x7f, 0xff, 0x7f)
  , RGB(0x7f, 0x7f, 0xff) 
  , RGB(0xff, 0xff, 0x7f)
  , RGB(0x7f, 0xff, 0xff)
  , RGB(0xff, 0x7f, 0xff)
};

void create_palette(std::vector<COLORREF>& cols)
{
  uint8_t val[] = { 0x60, 0x90, 0xc0, 0xf0};
  for(auto r : val)
  {
    for(auto g : val)
    {
      for (auto b : val)
      {
        cols.push_back(RGB(r,g,b));
      }
    }
  }
}

// void draw(std::vector<point_type> const & points
//  ,std::vector<group_info> const & groups)
// {
//   std::vector<COLORREF> cols;
//   create_palette(cols);

//   elt_type min[2]  {std::numeric_limits<elt_type>::max(), std::numeric_limits<elt_type>::max()};
//   elt_type max[2]  {std::numeric_limits<elt_type>::min(), std::numeric_limits<elt_type>::min()};
//   for(auto const & e: points)
//   {
//     min[0] = std::min(min[0], e[0]);
//     min[1] = std::min(min[1], e[1]);
//     max[0] = std::max(max[0], e[0]);
//     max[1] = std::max(max[1], e[1]);
//   }
//   auto img_width = max[0] - min[0];
//   auto img_height = max[1] - min[1];
//     // std::cout << "img_width " << img_width << " img_height " << img_height << std::endl;
//   point_type center_point { (min[0] + max[0]) / 2, (min[1] + max[1]) / 2};

//   HWND hwindow = GetConsoleWindow();
//   WINDOWINFO wi;
//   GetWindowInfo(hwindow, &wi);
//   auto [top, left, width, height] = wi.rcClient;
//   elt_type scale = std::min(width, height) / std::max(img_width, img_height);
//     // std::cout << "scale " << scale << std::endl;
//     // std::cout << "group_cols_len " << group_cols_len << std::endl;
//     // std::cout.flush();

//   HDC dc = GetDC(hwindow);
//   size_t i = 0;
//   for (auto const & e : points)
//   {
//     int x = scale * (e[0] - center_point[0]) + width / 2;
//     int y = scale * (e[1] - center_point[1]) + height / 2;
//     auto gr = groups[i].group_;
//     auto col = cols[gr % cols.size()];
//     // std::cout << e << " -> x " << x << " y " << y << std::endl;
//     SetPixel(dc, x, y, col); // RGB(0xa0, 0x90, 0x90)
//     ++i;
//   }
//   ReleaseDC(hwindow, dc);
// }

/**
 * just like draw, but using Gdiplus
 */
void draw2(std::vector<point_type> const & points
 , std::vector<group_info> const & groups
 , runtime_options const& opt
 , std::vector<center_info> * centers
 , tf::Executor* executor
 , bounding_rect const * bounds )
{

  std::vector<COLORREF> cols;
  create_palette(cols);

  bounding_rect br;

  if (nullptr == bounds)
  {
      bounding_rect tmp(points);
      br = tmp;
  }
  else {
      br = *bounds;
  }

  auto img_width = br.max[0] - br.min[0];
  auto img_height = br.max[1] - br.min[1];
    // std::cout << "img_width " << img_width << " img_height " << img_height << std::endl;
  point_type center_point { (br.min[0] + br.max[0]) / 2, (br.min[1] + br.max[1]) / 2};

  HWND hwindow = GetConsoleWindow();
  WINDOWINFO wi;
  GetWindowInfo(hwindow, &wi);
  auto width = wi.rcClient.right - wi.rcClient.left;
  auto height = wi.rcClient.bottom - wi.rcClient.top;
  elt_type scale = std::min(width, height) / std::max(img_width, img_height);
  
  auto transform = [center_point, width, height, scale](point_type p) {
      p -= center_point;
      return std::make_tuple( (int)(scale * (p[0]) + width / 2)
                            , (int)(scale * (p[1]) + height / 2));
  };

  HDC dc = GetDC(hwindow);
  Gdiplus::Graphics graphics(dc);
  Gdiplus::Bitmap bmp( width, height, &graphics);
  Gdiplus::BitmapData bmpdata;
  Gdiplus::Rect r(0,0, bmp.GetWidth(), bmp.GetHeight());
  using namespace Gdiplus;
  auto st = bmp.LockBits( &r
    , ImageLockModeWrite
    , PixelFormat32bppRGB
    , &bmpdata
    );
  if (st != Ok)
  {
    std::cout << "LockBits ist nicht ok" << std::endl;
  }
  uint32_t* pixels = (uint32_t*)bmpdata.Scan0;
  auto set_pixel = [pixels, bmpdata](size_t x, size_t y, COLORREF& col)
  {
    pixels[(bmpdata.Stride >> 2) * y + x] = col;
  };
  size_t i = 0;
  if (executor == nullptr) {
    for (auto const & e : points)
    {
      //int x = (int)(scale * (e[0] - center_point[0]) + width / 2);
      //int y = (int)(scale * (e[1] - center_point[1]) + height / 2);
      auto [x, y] = transform(e);
      auto gr = groups[i].group_;
      auto col = cols[gr % cols.size()];
      // std::cout << e << " -> x " << x << " y " << y << std::endl;
      set_pixel(x, y, col); // RGB(0xa0, 0x90, 0x90)
      ++i;
    }
  } else {
    tf::Taskflow taskflow;
    taskflow.parallel_for((int)0, (int)points.size(), 1, [&](int i) {
      auto const & e = points[i];
      //int x = (int)(scale * (e[0] - center_point[0]) + width / 2);
      //int y = (int)(scale * (e[1] - center_point[1]) + height / 2);
      auto [x, y] = transform(e);
      auto gr = groups[i].group_;
      auto col = cols[gr % cols.size()];
      // std::cout << e << " -> x " << x << " y " << y << std::endl;
      set_pixel(x, y, col); // RGB(0xa0, 0x90, 0x90)
    }, opt.draw_parallel_partitions);
    executor->run(taskflow).get();

  }
  bmp.UnlockBits(&bmpdata);
  graphics.DrawImage(&bmp, 0, 0);
  if (centers != nullptr)
  {
	  // std::cout << "Drawing centers" << std::endl;
	  size_t i = 0;
	  //Graphics img_graphics(&bmp);
	  Pen pen(Color(200, 0xff, 0xff, 0xff), 3);
      Pen pen2(Color(200, 0xff, 0xff, 0xff), 1);
	  for (auto const & e : *centers)
	  {
		  int x = (int)(scale * (e.point_[0] - center_point[0]) + width / 2);
		  int y = (int)(scale * (e.point_[1] - center_point[1]) + height / 2);
		  //auto col = cols[i % cols.size()];
          //auto c = cols[i % cols.size()];
		  //pen.SetColor(Color(c & 0xff, (c >> 8) & 0xff, (c >>16) & 0xff));
      // pen.SetColor(cols[i % cols.size()]);
		  graphics.DrawLine(&pen, x - 5, y - 5, x + 5, y + 5);
		  graphics.DrawLine(&pen, x - 5, y + 5, x + 5, y - 5);
          //Gdiplus::Point points[] = { {0, y}, { x, y}, { x, 0} };
          //graphics.DrawLines(&pen2, points, 3);
		  ++i;
	  }
	  //graphics.Flush();
  }
  ReleaseDC(hwindow, dc);
}

void points_from_image(std::wstring const & file_name
                  , std::vector<point_type> & points)
{
  using namespace Gdiplus;
  Gdiplus::BitmapData bmpdata;
  Bitmap bmp(file_name.c_str());
  Gdiplus::Rect r(0,0, bmp.GetWidth(), bmp.GetHeight());
  auto st = bmp.LockBits( &r
    , ImageLockModeRead
    , PixelFormat32bppARGB 
    , &bmpdata
    );
  if (st != Ok)
  {
    std::cout << "LockBits ist nicht ok" << std::endl;
  }
  uint32_t* pixels = (uint32_t*)bmpdata.Scan0;
  for(unsigned y = 0; y < bmpdata.Height; ++y)
  {
    auto p = pixels + y * (bmpdata.Stride >> 2);
    for(unsigned x = 0; x < bmpdata.Width; ++x, ++p)
    {
      Color col(*p);
      float fr = col.GetR() / 255.0f;
      float fg = col.GetG() / 255.0f;
      float fb = col.GetB() / 255.0f;
      float c_max = fr;
      float c_min = fr;
      int col_max = 1;
      if (fg > c_max)
      {
        col_max++;
        c_max = fg;
      }
      if (fg < c_min)
          c_min = fg;
      if (fb > c_max)
      {
        col_max++;
        c_max = fb;
      }
      if (fb < c_min)
          c_min = fb;
      constexpr const float deg60 = M_PI / 3.0f;
      float h = 0.0f;
      float delta = c_max - c_min;
      if (delta > 0.0f)
      {
        switch(col_max) {
          case 1: h = deg60 * std::fmod((fg - fb)/delta, 6.0f);
            break;
          case 2: h = deg60 * ((fb - fr)/delta + 2.0f);
            break;
          case 3: h = deg60 * ((fr - fg)/delta + 4.0f);
            break;
          default:
            h = 0.0f;
        }
      }
      points.push_back({c_max * std::cos(h), c_max * std::sin(h)});
    }
  }
  bmp.UnlockBits(&bmpdata);
}