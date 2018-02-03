//
// Created by Arnold on 30.01.2018.
//

#include "MazeBitmapPainter.h"
#include <windows.h>
#include <gdiplus.h>

namespace maze2 {

    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
        using namespace Gdiplus;
        using Gdiplus::DllExports::GdipGetImageEncoders;
        using Gdiplus::DllExports::GdipGetImageEncodersSize;
        UINT num = 0;          // number of image encoders
        UINT size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo *pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo * )(malloc(size));
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }
        }

        free(pImageCodecInfo);
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

//        {
//            // Test GDI+
//            using namespace Gdiplus;
//
//            GdiplusStartupInput gdiplusStartupInput;
//            ULONG_PTR           gdiplusToken;
//
//            // Initialize GDI+.
//            GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
//
//            CLSID clsid;
//            GetEncoderClsid(L"image/png", &clsid);
//
//            Bitmap gdibmp(bmp, NULL);
//            gdibmp.Save(path.data(), clsid);
//
//            GdiplusShutdown(gdiplusToken);
//        }
    }

}

bool maze2::RenderBitmap(const maze2::Maze &m, const size_t cellsize, DWORD pensize, DWORD padding, std::string prefix) {
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
    const DWORD clr = RGB(0x66, 0x00, 0x66);
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

    std::string filename = prefix + "_" + std::to_string(m.Width()) + "x" + std::to_string(m.Height()) + ".bmp";
    maze2::saveBitmap(filename, hbmp, hdc);

    DeleteObject(hpen);
    DeleteDC(hdc);
    DeleteObject(hbmp);

    return true;
}
