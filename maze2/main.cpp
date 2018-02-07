#include <iostream>

#include "maze.h"
#include "MazeBitmapPainter.h"

using namespace std;

int main(int argn, char* argv[]) {
    iostream::sync_with_stdio(false);
    try {
        maze2::pos_t x = 25, y;
        maze2::MazeDrawParams mazeDrawParams;
        int mark = 0;

        if (argn > 1) {
            x = stoul(argv[1]);
        }
        if (argn > 2) {
            y = stoul(argv[2]);
        } else {
            y = static_cast<maze2::pos_t>(x * std::sqrt(2));
        }
        if (argn > 3) {
            mazeDrawParams.cellsize = stol(argv[3]);
        }
        if (argn > 4) {
            mazeDrawParams.pensize = stol(argv[4]);
        }
        if (argn > 5) {
            mazeDrawParams.padding = stol(argv[5]);
        }
        if (argn > 6) {
            mark = stol(argv[6]);
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
        bool connected  = maze2::ShortestPath(mm, p1, p3, way);
        // bool connected2 = maze2::ShortestPath(mm, p2, p3, way);
        //if (connected && connected2) {
        if (connected) {
            //cout << p1 << ", " << p2 << ", and " << p3 << " are connected: ";
            cout << p1 << ", " << p3 << " are connected: " << endl;
            mm.SetMarked(p1);
            //mm.SetMarked(p2);
            mm.SetMarked(p3);

            if (mark)
                for (auto e : way)
                    mm.SetMarked(e);

            std::wstring filename = L"maze2_" + std::to_wstring(mm.Width()) + L"x" + std::to_wstring(mm.Height()) + L".png";
            maze2::RenderBitmapGDIP(mm, mazeDrawParams, filename);

            for(auto e : way) {
                cout << e << " ";
                    mm.SetMarked(e);
            }
            cout << endl;

            cout << "Schwierigkeit [1 - 100]: " << static_cast<unsigned>(way.size() / static_cast<double>(mm.Size()) * 100) << endl;

        } else {
            cout << p1 << ", " << p2 << ", and " << p3 << " are not connected." << endl;
            cout << p1 << " and " << p3 << " are not connected." << endl;
        }
        if (mm.Size() < 1000)
            cout << mm << endl;
        //cout.flush();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << endl;
    } catch (const char* e) {
        std::cerr << "Exception*: " << e << endl;
    }

    return 0;
}