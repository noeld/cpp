#include <iostream>

#include "maze.h"
#include "MazeBitmapPainter.h"

using namespace std;

int main(int argn, char* argv[]) {
    iostream::sync_with_stdio(false);
    try {
        maze2::pos_t x = 7, y = 4, cellsize = 20;
        int pen = 2, mark = 0;
        int padding = 5;
        if (argn > 1) {
            x = stoul(argv[1]);
        }
        if (argn > 2) {
            y = stoul(argv[2]);
        }
        if (argn > 3) {
            cellsize = stoul(argv[3]);
        }
        if (argn > 4) {
            pen = stoul(argv[4]);
        }
        if (argn > 5) {
            padding = stoul(argv[5]);
        }
        if (argn > 6) {
            mark = stoul(argv[6]);
        }
//    maze2::Maze m(8, 3);
//    cout << m;
//    cout << "RemoveWall(3, SOUTH)" << endl;
//    m.RemoveWall(3, maze2::SOUTH);
//    cout << m;
//    cout << "RemoveWall(3, WEST)" << endl;
//    m.RemoveWall(3, maze2::WEST);
//    cout << m;

        maze2::Maze mm(x, y);
        //cout << mm << endl;
        maze2::RandomizeMaze(mm);
        std::vector<maze2::pos_t> way;
        maze2::pos_t p1 = 0, p2 = mm.Size() / 2 + mm.Width() / 2, p3 = mm.Size() - 1;
        bool connected  = maze2::ShortestPath(mm, p1, p2, way);
        bool connected2 = maze2::ShortestPath(mm, p2, p3, way);
        if (connected && connected2) {
            cout << p1 << ", " << p2 << ", and " << p3 << " are connected: ";
            for(auto e : way) {
                cout << e << " ";
                if (mark)
                    mm.SetMarked(e);
            }
            cout << endl;
            mm.SetMarked(p1);
            mm.SetMarked(p2);
            mm.SetMarked(p3);
        } else {
            cout << p1 << ", " << p2 << ", and " << p3 << " are not connected." << endl;
        }
        cout << mm << endl;
        cout.flush();
        maze2::RenderBitmap(mm, cellsize, pen, padding);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << endl;
    } catch (const char* e) {
        std::cerr << "Exception*: " << e << endl;
    }

    return 0;
}