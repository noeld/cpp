#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <string>
#include <cmath>
#include "Point.hpp"
#include "Rect.hpp"


struct DiscreteIndex {
    template<typename T>
    explicit DiscreteIndex(T begin, T end, unsigned res_x = 100, unsigned res_y = 100)
    : res_x_ { res_x }, res_y_{ res_y}, map_(res_x * res_y, 0)
    {
        size_t ri = 0;
        if (begin != end)
            outline_ = *begin;
        while(begin != end) {
            const Rect &r = *begin;
            rects_.push_back(r);
            outline_ = Rect::combination(outline_, r);
            ++begin;
        }
        scalex_ = res_x_ / static_cast<double>(outline_.width());
        scaley_ = res_y_ / static_cast<double>(outline_.height());
        for(size_t i = 0; i < rects_.size(); ++i) {
            const Rect& r = rects_[i];
            auto index = i + 1;
            double x1 = ( r.p1_.x_ - outline_.p1_.x_ ) * scalex_;
            double y1 = ( r.p1_.y_ - outline_.p1_.y_ ) * scaley_;
            double x2 = x1 + r.width() * scalex_;
            double y2 = y1 + r.height() * scaley_;
            Rect pic(Point(std::floor(x1), std::floor(y1)), Point(std::ceil(x2), std::ceil(y2)));
            paint(pic, index);
        }
    }
    ~DiscreteIndex();

    std::vector<decltype(rects_.size())> map_;
    std::vector<decltype(rects_.size())>::value_type& get(unsigned x, unsigned y) {
        if (x < res_x_ && y < res_y_) {
            return map_.at(y * res_x_ + x);
        } else {
            throw "Out of range";
        }
    }

    void paint(const Rect& r, size_t i) {
        for(unsigned y = r.p1_.y_; y <= r.p2_.y_; ++y) {
            for(unsigned x = r.p1_.x_; x <= r.p2_.x_; ++x) {
                auto& e = get(x, y);
                if (e != 0) {

                } else {

                }
                combinations_.insert(std::make_pair(std::unordered_set{i}, ii);
                e = i;
            }
        }
    }

    Point<double> scale_world_to_index(const Point& p) const {
    	return Point( (p.x_ - outline_.p1_.x_) * scale_x_
    		        , (p.y_ - outline_.p1_.y_) * scale_y_
    		        );
    }

    Rect<int> scale_world_to_index(const Rect& r) const {
    	Point<double> p1 = scale_world_to_index(r.p1_);
    	Point<double> p2 = scale_world_to_index(r.p2_);
    	return Rect<int>(
    		Point<int>( std::max(std::floor(p1.x_), 0), std::max(std::floor(p1.y_), 0))
          , Point<int>( std::min(std::ceil(p2.x_), res_x_ - 1), std::min(std::ceil(p2.y_), res_y_ - 1))
    		       );
    }

    unsigned res_x_, res_y_;
    Rect outline_;
    double scalex_, scaley_;
    //using value_type = std::pair<Rect, size_t>
    std::vector<Rect> rects_;
    std::unordered_map<std::unordered_set<size_t>, size_t> combinations_;

};

struct CharPainter {
    explicit CharPainter(
			  const Rect& outline
			, unsigned cols = 100
			, unsigned rows = 30
			, std::ostream& io = std::cout)
	: offset_(outline.p1_)
	, cols_{cols}, rows_{rows}
	, scalex_(static_cast<float>(cols) / outline.width())
	, scaley_(static_cast<float>(rows) / outline.height())
	, io_{io}, s_(cols * rows, ' ')
	{}
	~CharPainter() {
		output();
	}
	void output() const {
		for(unsigned r = 0; r < rows_; ++r) {
			io_ << s_.substr(r * cols_, cols_) << std::endl;
		}
	}
	void paint(const Rect& r, char ch = '#') {
		Point a = r.p1_ - offset_;
		Point b = Point(a.x_ + scalex_ * r.width(), a.y_ + scaley_ * r.height());
		auto pic_rows = b.y_ - a.y_ + 1;
		auto pic_cols = b.x_ - a.x_ + 1;
		for(unsigned r = 0; r < pic_rows; ++r) {
			for(unsigned c = 0; c < pic_cols; ++c) {
				s_[(r + a.y_) * cols_ + c + a.x_] = ch;
			}
		}
	}
	const Point offset_;
	const float scalex_, scaley_;
	const unsigned cols_, rows_;
	std::ostream& io_;
	std::string s_;
};

void paint(const RTree::LeafNode& n, const unsigned maxcols, const unsigned maxrows) {
	Rect r;
	std::string s((maxcols+1)*maxrows, ' ');
	size_t i = 0;
	for(const auto& e : n.elements_) {
		if (i == 0)
			r = e.first;
		else
			r = Rect::combination(r, e.first);
		++i;
	}
	CharPainter p(Rect(Point(0, 0), Point(10, 10)));
	i = 0;
	for(const auto& e : n.elements_) {
		p.paint(e.first, '1' + ++i);
	}
}

int main(int argc, char const *argv[])
{
	std::vector<std::string> params(argv, argv + argc);
	RTree rt;
	for(const auto& p : params) {
		std::cout << p << std::endl;
	}
	size_t counter = 0;
	//rt.add(std::make_pair(Rect(Point(1, 1), Point(2, 2)), ++counter));
	//rt.add(std::make_pair(Rect(Point(1, 2), Point(2, 2)), ++counter));
	//rt.add(std::make_pair(Rect(Point(1, 3), Point(9, 2)), ++counter));
	//rt.add(std::make_pair(Rect(Point(2, 1), Point(4, 7)), ++counter));
	//rt.add(std::make_pair(Rect(Point(3, 1), Point(4, 7)), ++counter));
	rt.add(std::make_pair(Rect(Point(4, 1), Point(7, 2)), ++counter));
	rt.add(std::make_pair(Rect(Point(4, 4), Point(9, 8)), ++counter));
	rt.add(std::make_pair(Rect(Point(5, 1), Point(9, 9)), ++counter));
	rt.add(std::make_pair(Rect(Point(5, 4), Point(6, 5)), ++counter));
	rt.add(std::make_pair(Rect(Point(6, 1), Point(6, 6)), ++counter));
	rt.add(std::make_pair(Rect(Point(6, 6), Point(7, 7)), ++counter));
	//rt.add(std::make_pair(Rect(Point(1, 1), Point(7, 7)), ++counter));
	rt.add(std::make_pair(Rect(Point(8, 4), Point(9, 5)), ++counter));
	//rt.add(std::make_pair(Rect(Point(2, 5), Point(4, 6)), ++counter));
	//rt.add(std::make_pair(Rect(Point(2, 6), Point(6, 7)), ++counter));
	//rt.add(std::make_pair(Rect(Point(2, 7), Point(9, 7)), ++counter));
	rt.add(std::make_pair(Rect(Point(2, 8), Point(8, 9)), ++counter));
	paint(*dynamic_cast<RTree::LeafNode*>(rt.root_.get()), 100, 30);
	return 0;
}