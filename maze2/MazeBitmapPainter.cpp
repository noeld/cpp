//
// Created by Arnold on 30.01.2018.
//

#include "MazeBitmapPainter.h"
#include <windows.h>
#include <gdiplus.h>
#include <d2d1.h>
#include <complex>
#include <thread>

namespace maze2 {

    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
        using namespace Gdiplus;
        using Gdiplus::DllExports::GdipGetImageEncoders;
        using Gdiplus::DllExports::GdipGetImageEncodersSize;
        UINT num = 0;          // number of image encoders
        UINT size = 0;         // size of the image encoder array in bytes

        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        auto pMem = std::make_unique<char[]>(size);
        auto pImageCodecInfo = reinterpret_cast<ImageCodecInfo *>(pMem.get());
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[j].Clsid;
                return j;  // Success
            }
        }
        return -1;  // Failure
    }

    void saveBitmap( std::string path, HBITMAP bmp, HDC hdc )
    {
        BITMAPFILEHEADER fileheader;
        BITMAPINFO	 infoheader;
        BITMAP		 bitmap;
        DWORD		 wb;

        GetObject( bmp, sizeof( bitmap ), &bitmap );

        DWORD* dwpBits = new DWORD[bitmap.bmWidth * bitmap.bmHeight];
        ZeroMemory( dwpBits, bitmap.bmWidth * bitmap.bmHeight * sizeof( DWORD ) );
        ZeroMemory( &infoheader, sizeof( BITMAPINFO ) );
        ZeroMemory( &fileheader, sizeof( BITMAPFILEHEADER ) );

        infoheader.bmiHeader.biBitCount = sizeof( DWORD ) * 8;
        infoheader.bmiHeader.biCompression = BI_RGB;
        infoheader.bmiHeader.biPlanes = 1;
        infoheader.bmiHeader.biSize = sizeof( infoheader.bmiHeader );
        infoheader.bmiHeader.biHeight = bitmap.bmHeight;
        infoheader.bmiHeader.biWidth = bitmap.bmWidth;
        infoheader.bmiHeader.biSizeImage = bitmap.bmWidth * bitmap.bmHeight * sizeof( DWORD );

        fileheader.bfType    = 0x4D42;
        fileheader.bfOffBits = sizeof( infoheader.bmiHeader ) + sizeof( BITMAPFILEHEADER );
        fileheader.bfSize    = fileheader.bfOffBits + infoheader.bmiHeader.biSizeImage;

        GetDIBits( hdc, bmp, 0, bitmap.bmHeight, ( LPVOID )dwpBits, &infoheader, DIB_RGB_COLORS );

        HANDLE file = CreateFile( path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        WriteFile( file, &fileheader, sizeof( BITMAPFILEHEADER ), &wb, NULL );
        WriteFile( file, &infoheader.bmiHeader, sizeof( infoheader.bmiHeader ), &wb, NULL );
        WriteFile( file, dwpBits, bitmap.bmWidth * bitmap.bmHeight * 4, &wb, NULL );
        CloseHandle( file );

        delete [] dwpBits;
    }

}

bool maze2::RenderBitmap(const maze2::Maze &m, const size_t cellsize, DWORD pensize, DWORD padding, std::string filename) {
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize        = sizeof(bi.bmiHeader);
    bi.bmiHeader.biBitCount    = sizeof(DWORD) * 8;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biPlanes      = 1;
    const LONG bmpWidth        = cellsize * m.Width()  + pensize + 2 * padding;
    const LONG bmpHeight       = cellsize * m.Height() + pensize + 2 * padding;
    bi.bmiHeader.biWidth       = bmpWidth;
    bi.bmiHeader.biHeight      =-bmpHeight;
    HDC dc = GetDC( GetConsoleWindow());
    void* pBits;
    HBITMAP hbmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hbmp) return false;

    HDC hdc = CreateCompatibleDC(dc);
    SelectObject(hdc, hbmp);
    ReleaseDC(GetConsoleWindow(), dc);

    memset(pBits, 0xff, bmpWidth * bmpHeight * sizeof(DWORD));
    const DWORD clr = RGB(0x00, 0x66, 0x66);
    HPEN hpen = CreatePen(PS_SOLID, pensize, clr);
    SelectObject(hdc, hpen);

    const DWORD extraw = 0;
    PosNavigator pn(m.Width(), m.Height());
    for(maze2::pos_t y = 0; y < m.Height(); ++y) {
        int py = cellsize * y + padding;
        for(maze2::pos_t x = 0; x < m.Width(); ++x) {
            int px = cellsize * x + padding;
            auto pos = pn.ConvertToPos(x, y);
            if (m.HasWall(pos, maze2::NORTH)) {
                MoveToEx(hdc, px, py, nullptr);
                LineTo(hdc, px + cellsize + extraw, py);
            }
            if (m.HasWall(pos, maze2::EAST)) {
                MoveToEx(hdc, px + cellsize + extraw, py, nullptr);
                LineTo(hdc, px + cellsize + extraw, py + cellsize + extraw);
            }
            if (m.HasWall(pos, maze2::SOUTH)) {
                MoveToEx(hdc, px, py + cellsize + extraw, nullptr);
                LineTo(hdc, px + cellsize + extraw, py + cellsize + extraw);
            }
            if (m.HasWall(pos, maze2::WEST)) {
                MoveToEx(hdc, px, py, nullptr);
                LineTo(hdc, px, py + cellsize + extraw);
            }
            if (m.Marked(pos)) {
                Ellipse(hdc, px + cellsize / 2 - pensize, py + cellsize / 2 - pensize, px + cellsize / 2 + pensize, py + cellsize / 2 + pensize);
            }
        }
    }

    //ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
    //BitBlt(GetDC(GetConsoleWindow()), 20, 60, bmpWidth, bmpHeight, hdc, 0, 0, SRCCOPY);

    maze2::saveBitmap(filename, hbmp, hdc);

    DeleteObject(hpen);
    DeleteDC(hdc);
    DeleteObject(hbmp);

    return true;
}

using float_t   = float;
using complex_t = std::complex<float_t>;


static const unsigned maxit = 127;

unsigned Iterate(float_t x, float_t y) {
    complex_t c(x, y), cc;
    for(unsigned i = 0; i < maxit; ++i) {
        if (std::norm(cc) > 4.0) {
            return i;
        }
        cc = cc * cc + c;
    }
    return maxit;
};

void RenderMandelbrot(Gdiplus::Bitmap* bmp) {
    using namespace Gdiplus;

    const float_t xmin = -2.0f;
    const float_t ymin = -1.0f;
    const float_t xmax =  0.0f;
    const float_t ymax =  1.0f;

    const float_t xp = (xmax - xmin) / bmp->GetWidth();
    const float_t yp = (ymax - ymin) / bmp->GetHeight();

    auto compute = [bmp, xp, yp, xmin, ymin, xmax, ymax](UINT startnum, UINT num) {
//        for(UINT y = 0; y < bmp.GetHeight(); ++y) {
//            float_t yy = ymax - yp/2 - y * yp;
//            float_t xx = xmin + xp/2;
//            for(UINT x = 0; x < bmp.GetWidth(); ++x, xx += xp) {
//                unsigned iterations = it(xx, yy) + 127;
//                bmp.SetPixel(x, y, Color(iterations, iterations, iterations));
//            }
//        }
        UINT done = 0;
        unsigned w = bmp->GetWidth();
        unsigned x = startnum % w;
        unsigned y = startnum / w;
        float_t xx = xmin + xp/2 + x * xp;
        float_t yy = ymax - yp/2 - y * yp;
        while(done < num) {
            unsigned iterations = Iterate(xx, yy) + 127;
            bmp->SetPixel(x, y, Color(iterations, iterations, iterations));
            ++x;
            xx += xp;
            if (x == w) {
                x = 0;
                xx = xmin + xp / 2;
                ++y;
                yy -= yp;
            }
            ++done;
        }
    };

    std::vector<std::thread> threads;
    for(unsigned i = 0, n = 0, anz = (bmp->GetWidth() * bmp->GetHeight()) /  1 ;//std::thread::hardware_concurrency();
        n < 1; //std::thread::hardware_concurrency();
        ++n, i += anz) {
        //threads.emplace_back(compute, i, anz);
        compute(i, anz);
    }
    for(auto & e : threads) {
        e.join();
    }
}

bool maze2::RenderBitmapGDIP(const maze2::Maze &m, const MazeDrawParams& params, std::wstring filename) {
    // Test GDI+
    using namespace Gdiplus;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    CLSID clsid;
    GetEncoderClsid(L"image/png", &clsid);

    const INT bmpWidth        = params.cellsize * m.Width()  + params.pensize + 2 * params.padding;
    const INT bmpHeight       = params.cellsize * m.Height() + params.pensize + 2 * params.padding;
    {
        Bitmap bmp(bmpWidth, bmpHeight, PixelFormat24bppRGB);
        Graphics g(&bmp);

        g.Clear(Color(0xff, 0xff, 0xff));
        //RenderMandelbrot(&bmp);
        Pen p(Color(params.pen.r, params.pen.g, params.pen.b), params.pensize);
        PosNavigator pn(m.Width(), m.Height());
        for (maze2::pos_t y = 0; y < m.Height(); ++y) {
            int py = params.cellsize * y + params.padding;
            for (maze2::pos_t x = 0; x < m.Width(); ++x) {
                int px = params.cellsize * x + params.padding;
                auto pos = pn.ConvertToPos(x, y);
                if (m.HasWall(pos, maze2::NORTH)) {
                    g.DrawLine(&p, px, py, px + params.cellsize, py);
                }
                if (m.HasWall(pos, maze2::EAST)) {
                    g.DrawLine(&p, px + params.cellsize, py, px + params.cellsize, py + params.cellsize);
                }
                if (m.HasWall(pos, maze2::SOUTH)) {
                    g.DrawLine(&p, px, py + params.cellsize, px + params.cellsize, py + params.cellsize);
                }
                if (m.HasWall(pos, maze2::WEST)) {
                    g.DrawLine(&p, px, py, px, py + params.cellsize);
                }
                if (m.Marked(pos)) {
                    g.DrawEllipse(&p, px + params.cellsize / 2 - params.pensize / 2
                            , py + params.cellsize / 2 - params.pensize / 2
                            , params.pensize
                            , params.pensize);
                }
            }
        }
        const WCHAR * wFileName = filename.data();
        bmp.Save(wFileName, &clsid, nullptr);
    }
    GdiplusShutdown(gdiplusToken);

    return true;
}
