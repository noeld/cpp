#include <iostream>
#include <algorithm>
#include <tuple>
#include <complex>
#include <array>
#include <vector>
#include <memory>
#include <numeric>
#include <string>
#include <boost/format.hpp>

using namespace std;
using myfloat_t = double;
using c_t = complex<myfloat_t>;

template<typename T>
struct Rect {
  Rect(const T& xmin = T(), const T& ymin = T(), const T& xmax = T(), const T& ymax = T())
  : xmin_{xmin}, ymin_{ymin}, xmax_{xmax}, ymax_{ymax}
  {
    normalize();
  }
  Rect(const Rect&) = default;
  Rect& operator=(const Rect&) = default;
  friend bool operator==(const Rect<T>& a, const Rect<T>& b) { 
    return std::tie(a.xmin_, a.ymin_, a.xmax_, a.ymax_) == std::tie(b.xmin_, b.ymin_, b.xmax_, b.ymax_); 
  }
  friend bool operator<(const Rect<T>& a, const Rect<T>& b) { 
    return std::tie(a.xmin_, a.ymin_, a.xmax_, a.ymax_) < std::tie(b.xmin_, b.ymin_, b.xmax_, b.ymax_); 
  }
  friend bool operator<=(const Rect<T>& a, const Rect<T>& b) { 
    return (a < b) || (a == b); 
  }
  friend bool operator>(const Rect<T>& a, const Rect<T>& b) { 
    return (b < a); 
  }
  friend bool operator>=(const Rect<T>& a, const Rect<T>& b) { 
    return (b < a) || (a == b); 
  }
  friend bool operator!=(const Rect<T>& a, const Rect<T>& b) { 
    return !(a == b); 
  }
  friend std::ostream& operator<<(std::ostream& o, const Rect<T>& r) {
    std::string fmts = "[%6.5f, %6.5f, %6.5f, %6.5f]";
    if (std::is_integral<T>::value) {
      fmts = "[%d, %d, %d, %d]";
    }
    boost::format fmt{fmts};
    o << fmt % r.xmin_ % r.ymin_ % r.xmax_ % r.ymax_;
    return o;
  }
  void normalize() {
    if (xmax_ < xmin_)
      swap(xmax_, xmin_);
    if (ymax_ < ymin_)
      swap(ymax_, ymin_);
  }
  T width() const { 
    if (std::is_integral<T>::value)
      return xmax_ - xmin_ + 1; 
    else 
      return xmax_ - xmin_; 
  }
  T height() const { 
    if (std::is_integral<T>::value)
      return ymax_ - ymin_ + 1; 
    else 
      return ymax_ - ymin_; 
  }
  void zoom(myfloat_t f) {
    auto w = width();
    auto h = height();
    auto centerx = xmin_ + w/2.0;
    auto centery = ymin_ + h/2.0;
    auto nw = w * f;
    auto nh = h * f;
    xmin_ = centerx - nw/2.0;
    ymin_ = centery - nh/2.0;
    xmax_ = centerx + nw/2.0;
    ymax_ = centery + nh/2.0;
  }
  void move(const T& x, const T& y) {
    xmin_ += x;
    ymin_ += y;
    xmax_ += x;
    ymax_ += y;
  }
  T xmin_, ymin_, xmax_, ymax_;
};

using FRect = Rect<myfloat_t>;
using URect = Rect<unsigned>;

struct RenderParams {
  unsigned  maxit_  { 2000 };
  myfloat_t test_   { 4.0  };
  unsigned  width_  { 61   };
  unsigned  height_ { 23   };
  myfloat_t gamma_  { 0.6  };
  std::vector<char> alpha_ // { ' ', '.', ':', '-', '+', 'i', 'o', 'x', 'I', 'X', 'N', 'M', 'W', '%'};
  { ' ', '.', '\'', '`', '^', '"', ',', ':', ';', 'I', 'l', '!', 'i', '>', '<', '~', '+', '_', '-', '?'
  , ']', '[', '}', '{', '1', ')', '(', '|', '\\', '/', 't', 'f', 'j', 'r', 'x', 'n', 'u', 'v', 'c', 'z'
  , 'X', 'Y', 'U', 'J', 'C', 'L', 'Q', '0', 'O', 'Z', 'm', 'w', 'q', 'p', 'd', 'b', 'k', 'h', 'a', 'o'
  , '*', '#', 'M', 'W', '&', '8', '%', 'B', '@', '$'};
  friend std::ostream& operator<<(std::ostream& o, const RenderParams& rp) {
    boost::format fmt{"Iterations: %d  Test:%4.2f  Gamma: %4.3f"};
    o << fmt % rp.maxit_ % rp.test_ % rp.gamma_;
    return o;
  }
};

class MImage {
public:
  explicit MImage(unsigned width, unsigned height) 
  : width_{width}, height_{height}, rpp_{nullptr}
  , px_(width_ * height_, 0)
  {}
  unsigned& operator()(unsigned x, unsigned y) {
    return px_[y * width_ + x];
  }
  const unsigned& operator()(unsigned x, unsigned y) const {
    return px_[y * width_ + x];
  }
  unsigned width() const { return width_; }
  unsigned height() const { return height_; }
  MImage render(const RenderParams& param = RenderParams()) {
    MImage out(width(), height());
    out.rpp_ = std::unique_ptr<RenderParams>(new RenderParams(param));
    myfloat_t maxa = static_cast<myfloat_t>(param.alpha_.size() - 1);
    myfloat_t maxi = static_cast<myfloat_t>(param.maxit_);
    std::transform(cbegin(), cend(), out.begin()
      , [maxa, maxi, param](const decltype(*cbegin())& i) { return static_cast<unsigned>(pow(static_cast<myfloat_t>(i)/maxi, param.gamma_) * maxa); }
      );
    return out;
  }
  myfloat_t average() {
    auto acc = std::accumulate(cbegin(), cend(), 0);
    return static_cast<myfloat_t>(acc)/px_.size();
  }
  myfloat_t standard_derivation() {
    auto avg = average();
    myfloat_t res = 0;
    for(const auto& e : px_) {
      res += std::pow(static_cast<myfloat_t>(e) - avg, 2.0);
    }
    return std::sqrt(res);
  }
  friend std::ostream& operator<<(std::ostream& o, MImage& img) {
    if (img.rpp_) {
      unsigned n = 0;
      for(const auto& e : img) {
        o << img.rpp_->alpha_[e];
        ++n;
        if (n % img.width() == 0)
          o << std::endl;
      }
    } else {
      unsigned n = 0;
      for(const auto& e : img) {
        o << e << ' ';
        ++n;
        if (n % img.width() == 0)
          o << std::endl;
      }
    }
    return o;
  }
private:
  unsigned width_, height_;
  std::vector<unsigned> px_;
  std::unique_ptr<RenderParams> rpp_;
public:
  decltype(px_.begin()) begin() { return px_.begin(); }
  decltype(px_.end())   end()   { return px_.end(); }
  decltype(px_.cbegin()) cbegin() { return px_.cbegin(); }
  decltype(px_.cend())   cend()   { return px_.cend(); }
};

struct Mandelbrot {
  // Mandelbrot(const FRect& r)
  FRect r_ { -2.5, -1.5, 1.0, 1.5};
  MImage compute(const RenderParams& param = RenderParams()) const {
    MImage img(param.width_, param.height_);
    double fcols = r_.width()  / param.width_;
    double frows = r_.height() / param.height_;
    auto iit = img.begin(); 
    for(unsigned iy = 0; iy < param.height_; ++iy) {
      double y = r_.ymax_ - frows/2.0 - iy * frows;
      double x = r_.xmin_ + fcols/2.0;
      for(unsigned ix = 0; ix < param.width_; ++ix) {
        x += fcols;
        c_t c(x, y), cc(c);
        unsigned i;
        for(i = 0; norm(c) < param.test_ && i < param.maxit_ - 1; ++i) {
          c = c * c + cc;
          //c = pow(c, 2.6) + cc;
        }
        //img(ix, iy) = i;
        *iit = i;
        ++iit;
      }
    }
    return img;
  }

};

int main(int argn, char* argv[]) {
  ios_base::sync_with_stdio(false);
  boost::format fmt{"gamma: %6.3f standard_derivation: %8.4f"};
  Mandelbrot m;
  RenderParams rp;
  bool ok = true;
  std::string s;
  do {
    auto img = m.compute(rp);
    auto img2 = img.render(rp);
    std::cout << img2 << std::endl << m.r_ << rp << std::endl;
    std::cout << "Zoom + -  Move a w s d  Params t T i I g G r R c C Quit q:";
    std::cin  >> s;
    for (auto c : s) {
      switch(c) {
        case '+':
          m.r_.zoom(0.5);
          break;
        case '-':
          m.r_.zoom(2.0);
          break;
        case 'a':
          m.r_.move(- m.r_.width() * 0.25, 0);
          break;
        case 'd':
          m.r_.move(m.r_.width() * 0.25, 0);
          break;
        case 'w':
          m.r_.move(0, m.r_.height() * 0.25);
          break;
        case 's':
          m.r_.move(0, -m.r_.height() * 0.25);
          break;
        case 't':
          rp.test_ *= 2.0;
          break;
        case 'T':
          rp.test_ *= 0.5;
          break;
        case 'i':
          rp.maxit_ *= 2.0;
          break;
        case 'I':
          rp.maxit_ *= 0.5;
          break;
        case 'g':
          if (rp.gamma_ < 2.0)
            rp.gamma_ += 0.1;
          break;
        case 'G':
          if (rp.gamma_ > 0.1)
            rp.gamma_ -= 0.1;
          break;
        case 'c':
          rp.width_ += 4;
          break;
        case 'C':
          rp.width_ -= 4;
          break;
        case 'r':
          rp.height_ += 2;
          break;
        case 'R':
          rp.height_ -= 2;
          break;
        case 'q':
          ok = false;
          break;
      }
    }
    if(!ok)
      break;
  } while(1);
  // for(myfloat_t g = 1.2; g > 0.0; g-=0.1) {
  //   rp.gamma_ = g;
  //   auto img2 = img.render(rp);
  //   std::cout << fmt % g % img2.standard_derivation() << std::endl;
  //   std::cout << img2 << std::endl;
  // }
  return 0;
}