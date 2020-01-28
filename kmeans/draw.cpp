// http://supercomputingblog.com/graphics/getting-started-with-gdi-in-visual-studio/

#include "draw.h"
#include "group.h"
#include <iostream>

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
  uint8_t val[] = { 0x60, 0xa0, 0xf0};
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

void draw(std::vector<point_type> const & points
 ,std::vector<group_info> const & groups)
{
  std::vector<COLORREF> cols;
  create_palette(cols);

  elt_type min[2]  {std::numeric_limits<elt_type>::max(), std::numeric_limits<elt_type>::max()};
  elt_type max[2]  {std::numeric_limits<elt_type>::min(), std::numeric_limits<elt_type>::min()};
  for(auto const & e: points)
  {
    min[0] = std::min(min[0], e[0]);
    min[1] = std::min(min[1], e[1]);
    max[0] = std::max(max[0], e[0]);
    max[1] = std::max(max[1], e[1]);
  }
  auto img_width = max[0] - min[0];
  auto img_height = max[1] - min[1];
    // std::cout << "img_width " << img_width << " img_height " << img_height << std::endl;
  point_type center_point { (min[0] + max[0]) / 2, (min[1] + max[1]) / 2};

  HWND hwindow = GetConsoleWindow();
  WINDOWINFO wi;
  GetWindowInfo(hwindow, &wi);
  auto [top, left, width, height] = wi.rcClient;
  elt_type scale = std::min(width, height) / std::max(img_width, img_height);
    // std::cout << "scale " << scale << std::endl;
    // std::cout << "group_cols_len " << group_cols_len << std::endl;
    // std::cout.flush();

  HDC dc = GetDC(hwindow);
  size_t i = 0;
  for (auto const & e : points)
  {
    int x = scale * (e[0] - center_point[0]) + width / 2;
    int y = scale * (e[1] - center_point[1]) + height / 2;
    auto gr = groups[i].group_;
    auto col = cols[gr % cols.size()];
    // std::cout << e << " -> x " << x << " y " << y << std::endl;
    SetPixel(dc, x, y, col); // RGB(0xa0, 0x90, 0x90)
    ++i;
  }
  ReleaseDC(hwindow, dc);
}

/**
 * just like draw, but using Gdiplus
 */
void draw2(std::vector<point_type> const & points
 ,std::vector<group_info> const & groups)
{

  std::vector<COLORREF> cols;
  create_palette(cols);

  elt_type min[2]  {std::numeric_limits<elt_type>::max(), std::numeric_limits<elt_type>::max()};
  elt_type max[2]  {std::numeric_limits<elt_type>::min(), std::numeric_limits<elt_type>::min()};
  for(auto const & e: points)
  {
    min[0] = std::min(min[0], e[0]);
    min[1] = std::min(min[1], e[1]);
    max[0] = std::max(max[0], e[0]);
    max[1] = std::max(max[1], e[1]);
  }
  auto img_width = max[0] - min[0];
  auto img_height = max[1] - min[1];
    // std::cout << "img_width " << img_width << " img_height " << img_height << std::endl;
  point_type center_point { (min[0] + max[0]) / 2, (min[1] + max[1]) / 2};

  HWND hwindow = GetConsoleWindow();
  WINDOWINFO wi;
  GetWindowInfo(hwindow, &wi);
  auto width = wi.rcClient.right - wi.rcClient.left;
  auto height = wi.rcClient.bottom - wi.rcClient.top;
  elt_type scale = std::min(width, height) / std::max(img_width, img_height);
    // std::cout << "scale " << scale << std::endl;
    // std::cout << "group_cols_len " << group_cols_len << std::endl;
    // std::cout.flush();

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
  for (auto const & e : points)
  {
    int x = scale * (e[0] - center_point[0]) + width / 2;
    int y = scale * (e[1] - center_point[1]) + height / 2;
    auto gr = groups[i].group_;
    auto col = cols[gr % cols.size()];
    // std::cout << e << " -> x " << x << " y " << y << std::endl;
    set_pixel(x, y, col); // RGB(0xa0, 0x90, 0x90)
    ++i;
  }
  bmp.UnlockBits(&bmpdata);
  graphics.DrawImage(&bmp, 0, 0);
  ReleaseDC(hwindow, dc);
}