#prama once 

template<typename T = int>
struct Point final
{
	using value_type = T;
	explicit Point(const value_type& x = value_type(), const value_type& y = value_type()) noexcept
	: x_{x}, y_{y}
	{}
	Point(const Point&) noexcept = default;
	~Point() noexcept = default;
	Point& operator=(const Point&) noexcept = default;

	bool operator==(const Point& p) const noexcept {
		return std::tie(x_, y_) == std::tie(p.x_, p.y_);
	}
	bool operator!=(const Point& p) const noexcept { return !(p == *this); }

	bool operator<(const Point& p) const noexcept {
		return std::tie(x_, y_) < std::tie(p.x_, p.y_);
	}

	bool operator<=(const Point& p) const noexcept { return !(p < *this); }
	bool operator>=(const Point& p) const noexcept { return !(*this < p); }
	bool operator>(const Point& p) const noexcept  { return !(*this <= p); }

	Point& operator-=(const Point& p) noexcept { x_ -= p.x_; y_ -= p.y_; return *this; }
	friend Point operator-(const Point& p1, const Point& p2) {
		return Point(p1.x_ - p2.x_, p1.y_ - p2.y_);
	}
 	Point& operator+=(const Point& p) noexcept { x_ += p.x_; y_ += p.y_; return *this; }
	friend Point operator+(const Point& p1, const Point& p2) {
		return Point(p1.x_ + p2.x_, p1.y_ + p2.y_);
	}
	bool is_top_left(const Point& p) const noexcept {
		return x_ <= p.x_ && y_ <= p.y_;
	}
	/**
	 * Assert p1 is top left of p2
	 * i.e. p1.x <= p2.x and p1.y <= p2.y
	 */
	static void assert_top_left(Point& p1, Point& p2) {
		if (p1.x_ > p2.x_)
			std::swap(p1.x_, p2.x_);
		if (p1.y_ > p2.y_)
			std::swap(p1.y_, p2.y_);
	}

	value_type x_{0}, y_{0};
};
