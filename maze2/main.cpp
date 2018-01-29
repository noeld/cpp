#include <iostream>
#include <tuple>
#include <memory>
#include <vector>
#include <random>
#include <stack>

namespace maze2 {

    using pos_t = uint16_t;
    using dir_t = uint8_t;
    constexpr const dir_t NORTH = 1 << 0;
    constexpr const dir_t EAST  = 1 << 1;
    constexpr const dir_t SOUTH = 1 << 2;
    constexpr const dir_t WEST  = 1 << 3;
    constexpr const dir_t VISITED = 1 << 4;

    class PosNavigator final {
    public:
        explicit PosNavigator(pos_t maxx, pos_t maxy, pos_t cur = 0)
                : maxx_(maxx), maxy_(maxy), cur_(cur)
        {}
        ~PosNavigator() =default;
        PosNavigator(const PosNavigator&) = default;
        PosNavigator& operator=(const PosNavigator&) = default;

        bool operator==(const PosNavigator &rhs) const {
            return std::tie(maxx_, maxy_, cur_) == std::tie(rhs.maxx_, rhs.maxy_, rhs.cur_);
        }

        bool operator!=(const PosNavigator &rhs) const {
            return !(rhs == *this);
        }

        operator pos_t() const {
            return cur_;
        }
        PosNavigator& MoveDir(dir_t dir) {
            switch (dir) {
                case NORTH:
                    return MoveNorth();
                case EAST:
                    return MoveEast();
                case SOUTH:
                    return MoveSouth();
                case WEST:
                    return MoveWest();
                case 0:
                    break;
                default:
                    throw "Unknown direction";
            }
            return *this;
        }
        PosNavigator& SetPos(pos_t pos) {
            if (pos > maxx_ * maxy_ - 1)
                throw "To big pos";
            cur_ = pos;
            return *this;
        }
        PosNavigator& MoveEast() {
            if (cur_ % maxx_ < maxx_ - 1)
                ++cur_;
            return *this;
        }
        PosNavigator& MoveNorth() {
            if (cur_ >= maxx_)
                cur_ -= maxx_;
            return *this;
        }
        PosNavigator& MoveSouth() {
            if (cur_ < ((maxy_ - 1) * maxx_))
                cur_ += maxx_;
            return *this;
        }
        PosNavigator& MoveWest() {
            if (cur_ % maxx_ > 0)
                --cur_;
            return *this;
        }
        pos_t X() const { return cur_ % maxx_; }
        pos_t Y() const { return cur_ / maxx_; }
        void ConvertToXY(pos_t pos, pos_t& x, pos_t& y) const {
            x = pos % maxx_;
            y = pos / maxx_;
        }
        pos_t ConvertToPos(pos_t x, pos_t y) const {
            return y * maxx_ + x;
        }
    private:
        pos_t maxx_, maxy_;
        pos_t cur_;
    };

    class Maze final {
    public:
        Maze(const pos_t maxx, const pos_t maxy) : maxx_(maxx), maxy_(maxy), p_(Size(), NORTH | EAST)
        {}
        ~Maze() =default;
        void Initialize() {
            for(auto& e : p_) {
                e = NORTH | EAST;
            }
        }
        void SetVisited(pos_t pos) {
            p_[pos] |= VISITED;
        }
        bool Visited(pos_t pos) const {
            return (p_[pos] & VISITED) > 0;
        }
        bool HasWall(pos_t pos, dir_t dir) const {
            Normalize(pos, dir);
            return (p_[pos] & dir) == dir;
        }
        void RemoveWall(pos_t pos, dir_t dir) {
            Normalize(pos, dir);
            p_[pos] &= ~dir;
        }
        void BuildWall(pos_t pos, dir_t dir) {
            Normalize(pos, dir);
            p_[pos] |= dir;
        }
        pos_t Width() const { return maxx_; }
        pos_t Height() const { return maxy_; }
        pos_t Size() const { return maxx_ * maxy_; }
    private:
        void Normalize(pos_t& pos, dir_t& dir) const {
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
        const pos_t maxx_, maxy_;
        std::vector<uint8_t> p_;
    };

    void RandomizeMaze(Maze& m) {
        struct StackItem { pos_t pos {0}; dir_t dir {NORTH}; uint8_t cnt {0}; };
        std::random_device rd;
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
}

std::ostream& operator<<(std::ostream& o, const maze2::Maze& m) {
    using maze2::pos_t;
    const char* HWALL = "---";
    const char* HSPCE = "   ";
    maze2::PosNavigator pn(m.Width(), m.Height());
    for(pos_t y = 0; y < m.Height(); ++y) {
        for(pos_t x = 0; x < m.Width(); ++x) {
            o << '+' << (m.HasWall(pn.ConvertToPos(x, y), maze2::NORTH)?HWALL:HSPCE);
        }
        o << '+' << std::endl
          << (m.HasWall(pn.ConvertToPos(0, y), maze2::WEST)?'|':' ');
        for(pos_t x = 0; x < m.Width(); ++x) {
            o << HSPCE << (m.HasWall(pn.ConvertToPos(x, y), maze2::EAST)?'|':' ');
        }
        o << std::endl;
    }
    o << '+';
    for(pos_t x = 0; x < m.Width(); ++x) {
        o << (m.HasWall(pn.ConvertToPos(x, m.Height() - 1), maze2::SOUTH)?HWALL:HSPCE) << '+';
    }
    o << std::endl;
}

using namespace std;

int main(int argn, char* argv[]) {
    maze2::pos_t x = 7, y = 3;
    if (argn > 1) {
        x = stoul(argv[1]);
    }
    if (argn > 2) {
        y = stoul(argv[2]);
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
    cout << mm << endl;
    cout.flush();
    return 0;
}