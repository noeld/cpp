//
// Created by Arnold on 30.01.2018.
//

#ifndef MAZE2_MAZE_H
#define MAZE2_MAZE_H

#include <iostream>
#include <tuple>
#include <memory>
#include <vector>
#include <random>
#include <stack>

namespace maze2 {

    using pos_t = uint32_t;
    using dir_t = uint8_t;
    constexpr const dir_t NORTH = 1 << 0;
    constexpr const dir_t EAST  = 1 << 1;
    constexpr const dir_t SOUTH = 1 << 2;
    constexpr const dir_t WEST  = 1 << 3;
    constexpr const dir_t VISITED = 1 << 4;
    constexpr const dir_t MARKED  = 1 << 5;

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
        {
            if ((maxx_ * maxy_) == 0)
                throw "Not a valid maze layout";
        }
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
        void SetMarked(pos_t pos) {
            p_[pos] |= MARKED;
        }
        bool Marked(pos_t pos) const {
            return (p_[pos] & MARKED) > 0;
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
        void Normalize(pos_t& pos, dir_t& dir) const;
        const pos_t maxx_, maxy_;
        std::vector<uint8_t> p_;
    };

    void RandomizeMaze(Maze& m);
    bool ShortestPath(const Maze& m, pos_t start, pos_t end, std::vector<pos_t>& way);
}

std::ostream& operator<<(std::ostream& o, const maze2::Maze& m);



#endif //MAZE2_MAZE_H
