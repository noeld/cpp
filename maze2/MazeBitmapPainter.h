//
// Created by Arnold on 30.01.2018.
//

#ifndef MAZE2_MAZEBITMAPPAINTER_H
#define MAZE2_MAZEBITMAPPAINTER_H

#include "maze.h"

#include <minwindef.h>

namespace maze2 {

    bool RenderBitmap(const Maze& m, const size_t cellsize = 20, DWORD pensize = 2, DWORD padding = 2, std::string prefix = "maze");

}


#endif //MAZE2_MAZEBITMAPPAINTER_H
