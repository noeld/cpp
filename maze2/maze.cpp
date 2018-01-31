//
// Created by Arnold on 30.01.2018.
//

#include "maze.h"
#include <chrono>
#include <queue>
#include <algorithm>

void maze2::RandomizeMaze(maze2::Maze &m) {
    struct StackItem { pos_t pos {0}; dir_t dir {NORTH}; uint8_t cnt {0}; };
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rd (seed1);
    std::uniform_int_distribution<uint8_t> id(0, 3);
    std::uniform_int_distribution<pos_t> sd(0, m.Size() - 1);
    std::stack<StackItem> st;
    PosNavigator pn(m.Width(), m.Height());
    auto start = sd(rd);
    m.SetVisited(start);
    dir_t firstdir = (1 << id(rd));
    st.push({start, firstdir, 0});
    size_t maxsd = st.size();
    do {
        auto& e = st.top();
        do {
            ++e.cnt;
            e.dir <<= 1;
            if (e.dir > WEST)
                e.dir = NORTH;
        } while(e.cnt < 5 && m.Visited(pn.SetPos(e.pos).MoveDir(e.dir)));
        if (e.cnt >= 5) {
            st.pop();
        } else {
            m.SetVisited(pn);
            m.RemoveWall(e.pos, e.dir);
            dir_t firstdir = (1 << id(rd));
            st.push({pn, firstdir, 0});
            maxsd = std::max(maxsd, st.size());
        }
    } while(!st.empty());
    std::cerr << "Max stack depth = " << maxsd << std::endl;
}

std::ostream &operator<<(std::ostream &o, const maze2::Maze &m) {
    using maze2::pos_t;
    const char* HWALL = "--";
    const char* HSPCE = "  ";
    const char* HMRKD = "**";
    maze2::PosNavigator pn(m.Width(), m.Height());
    for(pos_t y = 0; y < m.Height(); ++y) {
        for(pos_t x = 0; x < m.Width(); ++x) {
            o << '+' << (m.HasWall(pn.ConvertToPos(x, y), maze2::NORTH)?HWALL:HSPCE);
        }
        o << '+' << std::endl
          << (m.HasWall(pn.ConvertToPos(0, y), maze2::WEST)?'|':' ');
        for(pos_t x = 0; x < m.Width(); ++x) {
            auto p = pn.ConvertToPos(x, y);
            o << (m.Marked(p)?HMRKD:HSPCE) << (m.HasWall(p, maze2::EAST)?'|':' ');
        }
        o << std::endl;
    }
    o << '+';
    for(pos_t x = 0; x < m.Width(); ++x) {
        o << (m.HasWall(pn.ConvertToPos(x, m.Height() - 1), maze2::SOUTH)?HWALL:HSPCE) << '+';
    }
    o << std::endl;
    return o;
}

void maze2::Maze::Normalize(maze2::pos_t &pos, maze2::dir_t &dir) const {
    if ((dir & (SOUTH | WEST)) > 0) {
        auto tmp = PosNavigator(maxx_, maxy_, pos).MoveDir(dir);
        if (pos != tmp) {
            pos = tmp;
            switch (dir) {
                case SOUTH:
                    dir = NORTH;
                    break;
                case WEST:
                    dir = EAST;
                    break;
                default:
                    break;
                    // nothing
            }
        } else {
            dir = 0;
        }
    }
}

bool maze2::ShortestPath(const Maze& m, pos_t start, pos_t dst, std::vector<pos_t>& way) {
    // https://de.wikipedia.org/wiki/Dijkstra-Algorithmus
    using dist_t = unsigned;
    constexpr const dist_t INF = std::numeric_limits<dist_t>::max();
    constexpr const pos_t  NOSUCHPOS = std::numeric_limits<pos_t>::max();
    using status_t = struct {
        dist_t distance    { INF   } ;
        pos_t  predecessor { NOSUCHPOS } ;
        bool   visited     { false } ;
    };
    // initialize
    auto status = std::make_unique<status_t[]>(m.Size());
    status[start].distance = 0;
    auto comp = [&status](const pos_t& a, const pos_t& b) { return status[a].distance < status[b].distance; };
    std::priority_queue<pos_t, std::vector<pos_t>, decltype(comp)> q(comp, std::vector<pos_t>());
    q.emplace(start);
    pos_t pos = 0;
    PosNavigator pn(m.Width(), m.Height());
    while(!q.empty()) {
        pos = q.top();
        q.pop();
        if (pos == dst)
            break;
        for(std::remove_cv<decltype(maze2::NORTH)>::type dir = 1; dir <= maze2::WEST; dir <<= 1) {
            if (m.HasWall(pos, dir))
                continue;
            pos_t neighbour = pn.SetPos(pos).MoveDir(dir);
            dist_t alt = status[pos].distance + 1;
            auto& st =  status[neighbour];
            if (alt < st.distance) {
                st.distance    = alt;
                st.predecessor = pos;
                if (!st.visited) {
                    st.visited = true;
                    q.emplace(neighbour);
                }
            }
        }
    }

    bool res = (pos == dst);
    if (res) {
        auto current_top = way.size();

        while(status[pos].predecessor != NOSUCHPOS) {
            way.push_back(pos);
            pos = status[pos].predecessor;
        }
        way.push_back(start);
        std::reverse(way.begin() + current_top, way.end());
    }
    return res;
}
