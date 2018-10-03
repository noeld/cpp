/* areagrid - a geometric data structure which splits an area into rectangles
*/
#include <iostream>
#include <tuple>
#include <array>
#include <algorithm>
#include <climits>
#include <random>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

struct EquiDistRange {
    EquiDistRange(int min, int max, int divisions)
    : min_{std::min(min, max)}, max_{std::max(min, max)}
    , dist_{max_ - min_}
    , divisions_{divisions}
    , d_{static_cast<double>((dist_)/divisions_)}
    {}
    int operator[](int i) const {
        return static_cast<int>(min_ + d_ * i);
    }
    struct Iterator { 
        const EquiDistRange& r_;
        int index_ { 0 };
        bool operator==(const Iterator& b) const { return index_ == b.index_; }
        bool operator!=(const Iterator& b) const { return !(*this == b); }
        bool operator<(const Iterator& b) const { return index_ < b.index_; }
        bool operator<=(const Iterator& b) const { return index_ <= b.index_; }
        bool operator>(const Iterator& b) const { return index_ > b.index_; }
        bool operator>=(const Iterator& b) const { return index_ >= b.index_; }
        Iterator& operator++() { ++index_; }
        Iterator operator++(int) { auto tmp = *this; ++index_; return tmp; }
        Iterator& operator--() { --index_; }
        Iterator operator--(int) { auto tmp = *this; --index_; return tmp; }
        int operator*() { return r_[index_]; }
    };
    Iterator begin() const { return Iterator{*this}; }
    Iterator end() const { return Iterator{*this, divisions_ + 1}; }
    const int min_, max_, dist_, divisions_;
    double d_;
};

struct Point {
  int x, y;
  bool is_top_left(const Point& other) const noexcept {
    return x < other.x && y < other.y;
  }
  bool is_top_left_eq(const Point& other) const noexcept {
    return x <= other.x && y <= other.y;
  }
  bool operator==(const Point& p) const noexcept {
    return x == p.x && y == p.y;
  }
};


std::ostream& operator<<(std::ostream& o, const Point& p) {
  return o << '(' << p.x << ", " << p.y << ')';
}

struct Rect {
  Point min, max;
  void sanitize() {
    if (min.x > max.x)
      std::swap(min.x, max.x);
    if (min.y > max.y)
      std::swap(min.y, max.y);
  }
  bool operator==(const Rect& o) const noexcept { return min == o.min && max == o.max; }
  bool contains(const Point& p) const noexcept {
    return min.is_top_left_eq(p) && p.is_top_left_eq(max);
  }
  int width() const noexcept { return max.x - min.x + 1; }
  int height() const noexcept { return max.y - min.y + 1; }
  int area() const noexcept { return width() * height(); }
  bool intersects(const Rect& r, Rect* intersection = nullptr) const noexcept {
    Rect tmp{ std::max(min.x, r.min.x)
            , std::max(min.y, r.min.y)
            , std::min(max.x, r.max.x)
            , std::min(max.y, r.max.y)
            };
    auto tl = tmp.min.is_top_left_eq(tmp.max);
    if (tl && intersection != nullptr) {
      *intersection = tmp;
    }
    return tl;
  }
  Point center() const noexcept { return Point{ min.x + width()/2, min.y + height()/2};}
  template<typename T>
  static 
  Rect bounding_rect(T begin, const T end) {
    Rect tmp = { INT_MAX, INT_MAX, INT_MIN, INT_MIN};
    for(;begin != end; ++begin) {
      const Rect* ptr = static_cast<const Rect*>(*begin);
      tmp.min.x = std::min(ptr->min.x, tmp.min.x);
      tmp.min.y = std::min(ptr->min.y, tmp.min.y);
      tmp.max.x = std::max(ptr->max.x, tmp.max.x);
      tmp.max.y = std::max(ptr->max.y, tmp.max.y);
    }
    return tmp;
  }
  template<typename T>
  static 
  bool intersect(T begin, const T end, Rect* intersection = nullptr) {
    Rect tmp = { INT_MIN, INT_MIN, INT_MAX, INT_MAX};
    for(;begin != end; ++begin) {
      const Rect* ptr = static_cast<const Rect*>(*begin);
      if (!tmp.intersects(*ptr, &tmp))
        return false;
    }
    if (intersection != nullptr) {
      *intersection = tmp;
    }
    return true;
  }
};

class Randomizer {
public:
  Randomizer(unsigned area_width  = 1000
           , unsigned area_height = 1000
           , unsigned max_width   =  200
           , unsigned max_height  =  200) 
  : distx(0, area_width - 1)
  , disty(0, area_height - 1)
  , max_width_{max_width}
  , max_height_{max_height}
  {
    if (area_width < max_width || area_height < max_height)
        throw "Randomizer: Illegal dimensions";
  }
  void operator()(Point& p) {
    p.x = distx(rd);
    p.y = disty(rd);
  }
  void operator()(Rect& r) {
    operator()(r.min);
    std::uniform_int_distribution<unsigned> distw(0, std::min(max_width_, distx.max() - r.min.x));
    std::uniform_int_distribution<unsigned> disth(0, std::min(max_height_, disty.max() - r.min.y));
    r.max.x = r.min.x + distw(rd);
    r.max.y = r.min.y + disth(rd);
    r.sanitize();
  }
private:
  std::random_device rd;
  std::uniform_int_distribution<unsigned> distx;
  std::uniform_int_distribution<unsigned> disty;
  unsigned max_width_, max_height_;
};

struct RectScale {
    RectScale(const Rect& bounds, unsigned width = 100, unsigned height = 100)
    : bounds_{bounds}
    , width_{width}
    , height_{height} 
    , fx_{width_/static_cast<double>(bounds_.width())}
    , fy_{height_/static_cast<double>(bounds_.height())}
    {}
    const Rect bounds_;
    const unsigned width_, height_;
    const double fx_, fy_;
    Rect operator()(const Rect& r) const {
        return Rect{
            static_cast<int>(std::floor(fx_ * ( r.min.x - bounds_.min.x)))
          , static_cast<int>(std::floor(fy_ * ( r.min.y - bounds_.min.y)))
          , static_cast<int>(std::ceil(fx_ * ( r.max.x - bounds_.min.x)))
          , static_cast<int>(std::ceil(fy_ * ( r.max.y - bounds_.min.y)))
        };
    }
    Point operator()(const Point& p) const {
      return Point{
        static_cast<int>(fx_ * (p.x - bounds_.min.x))
      , static_cast<int>(fy_ * (p.y - bounds_.min.y))
      };
    }
};

std::ostream& operator<<(std::ostream& o, const Rect& r) {
  return o << '[' << r.min << ", " << r.max << ']';
}

bool operator<(const Point& a, const Point& b) {
  return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

bool operator<(const Rect& a, const Rect& b) {
  return std::tie(a.min, a.max) < std::tie(b.min, b.max);
}

struct VRect {
  Rect r;
  int value;
  operator Rect*() noexcept { return &r; }
  bool operator==(const VRect& o) const noexcept { return r == o.r && value == o.value;}
};
std::ostream& operator<<(std::ostream& o, const VRect& r) {
  return o << '[' << r.value << ", " << r.r.min << ", " << r.r.max << ']';
}
bool operator<(const VRect& a, const VRect& b) {
  return a < b;
}

template<typename T>
void print_all(const T& l) {
  for(const auto& e : l) {
    std::cout << e << std::endl;
  }
}

namespace std
{
  template<> struct hash<Point>
  {
    typedef Point argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
      result_type const h1 ( std::hash<int>{}(s.x) );
      result_type const h2 ( std::hash<int>{}(s.y) );
      return h1 ^ (h2 << 1); 
    }
  };
  template<> struct hash<Rect>
  {
    typedef Rect argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
      result_type const h1 ( std::hash<Point>{}(s.min) );
      result_type const h2 ( std::hash<Point>{}(s.max) );
      return h1 ^ (h2 << 1); 
    }
  };
  template<> struct hash<VRect>
  {
    typedef VRect argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
      result_type const h1 ( std::hash<Rect>{}(s.r) );
      result_type const h2 ( std::hash<int>{}(s.value) );
      return h1 ^ (h2 << 1); 
    }
  };
}

class VRectGrid {
public:
  using set_t = std::unordered_set<VRect>;
  using map_t = std::unordered_map<Point, set_t>;
  template<typename T>
  VRectGrid(const RectScale& scale, T begin, const T end);
  const set_t& operator()(int x, int y);
  const set_t& operator()(const Point&);
  const RectScale scale_;
private:
  map_t map_;
};

template<typename T>
VRectGrid::VRectGrid(const RectScale& scale, T begin, const T end)
: scale_{scale}
{
  for(int y = 0; y <= scale_.height_ + 1; ++y) {
    for(int x = 0; x <= scale_.width_ + 1; ++x) {
      Point p{x, y};
      set_t set;
      for(auto it = begin; it != end; ++it) {
        if (scale_(it->r).contains(p)) {
          set.insert(*it);
        }
      }
      map_.insert({p, set});
    }
  }
}
const VRectGrid::set_t& VRectGrid::operator()(const Point& p) {
  return map_[scale_(p)];
}
const VRectGrid::set_t& VRectGrid::operator()(int x, int y) {
  return this->operator()(Point{x, y});
}

int main() {
  using namespace std;
  std::array<VRect, 20> rs;
  Randomizer rnd;
  int idx = 1;
  for(auto & e : rs) {
    e.value = idx++;
    rnd(e.r);
  }
  print_all(rs);
  std::sort(rs.begin(), rs.end(), [](const VRect& a, const VRect& b) { return a.r < b.r;});
  std::cout << "***********" << std::endl;
  print_all(rs);
  Rect bounds = Rect::bounding_rect(rs.begin(), rs.end());
  std::cout << "*" << bounds << std::endl;

  const int buckets = 5;
  float rects_per_bucket = rs.size() / (float)buckets;
  float dx = static_cast<float>(bounds.width())/rs.size();
  for(size_t i = 0; i < rs.size(); ++i) {
    int current_x = dx * i;
    int bucket = i / rects_per_bucket;
    cout << i << " " << current_x << " " << bucket 
         << " " << rs[i].r.min.x 
         << " " << rs[i] << rs[i].r.center()
         << endl;
  }
  auto b = rs.begin();
  int bb = 0;
  for(size_t i = 0; ; ++i) {
    int bucket = i / rects_per_bucket;
    if (bb < bucket) {
      Rect intersection;
      auto intersect = Rect::intersect(b, rs.begin() + i, &intersection);
      cout << "Bucket " << bb << " intersect: " 
           << boolalpha << intersect << ": " << intersection 
           << static_cast<float>(intersection.area())/bounds.area()/buckets << endl;
      bb = bucket;
      b = rs.begin() + i;
    }
    if (i >= rs.size())
      break;
  }
  EquiDistRange r{0, 8, 3};
  for(auto i : r) {
    cout << i << ", ";
  }
  cout << endl;
  RectScale scale(bounds, 50, 50);
  VRectGrid vrg{scale, rs.begin(), rs.end()};
  Point p;
  for(int i = 0; i < 10; ++i) {
    rnd(p);
    cout << "**** Loopkup " << p << endl;
    for(auto& e : vrg(p)) {
      cout << e << " " << e.r.contains(p) << endl;
    }
  }
}