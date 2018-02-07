//
// Created by Arnold on 30.01.2018.
//

#ifndef MAZE2_MAZEBITMAPPAINTER_H
#define MAZE2_MAZEBITMAPPAINTER_H

#include "maze.h"

#include <minwindef.h>

namespace maze2 {

    struct MazeDrawParams {
        int cellsize  { 30 };
        int padding   { cellsize };
        int pensize   { 3 };
        struct {
            BYTE r    { 0x33 };
            BYTE g    { 0x00 };
            BYTE b    { 0x33 };
        } pen;
    };

    bool RenderBitmap(const Maze& m, const size_t cellsize = 20, DWORD pensize = 2, DWORD padding = 2, std::string prefix = "maze");
    bool RenderBitmapGDIP(const maze2::Maze &m, const MazeDrawParams& params, std::wstring prefix = L"maze2.png");
}


#endif //MAZE2_MAZEBITMAPPAINTER_H
