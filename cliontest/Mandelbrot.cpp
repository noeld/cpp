//
// Created by Arnold Müller on 10.02.17.
//

#include <complex>
#include "Mandelbrot.h"


Mandelbrot::Mandelbrot() : out_ { std::cout }
{
    range_screen_.xmax_ = 100;
    range_window_.fit(range_screen_);
    range_window_.scale(0.9);
}

Mandelbrot::Mandelbrot(const DoubleRange &window, const DoubleRange &screen, std::ostream& out)
        : range_window_(window), range_screen_(screen), out_(out)
{
    range_window_.fit(range_screen_);
    range_window_.scale(0.9);
}

void Mandelbrot::run() const {
    double stepx = range_window_.width() / range_screen_.width();
    double stepy = range_window_.height() / range_screen_.height();
    for(double y = range_window_.ymax_; y > range_window_.ymin_; y -= stepy) {
        for(double x = range_window_.xmin_; x < range_window_.xmax_; x += stepx) {
            //std::cout << "y: " << y << " x: " << x << std::endl;
            auto i = iter(x, y, max_it_, p_);
            out_ << alphaFor(i);
        }
        out_ << std::endl;
    }
}

unsigned int Mandelbrot::iter(const double& x, const double& y, unsigned int maxit, const double& p) {
    using complex_t = std::complex<double>;
    complex_t c(x,y), cc(c);
    for(unsigned i = 0; i < maxit; ++i) {
        cc = std::pow(cc, p) + c;
        if (std::abs(cc) > 4.0)
            return i;
    }
    return maxit - 1;
}

const DoubleRange &Mandelbrot::getRange_window() const {
    return range_window_;
}

void Mandelbrot::setRange_window(const DoubleRange &range_window_) {
    Mandelbrot::range_window_ = range_window_;
}

const DoubleRange &Mandelbrot::getRange_screen() const {
    return range_screen_;
}

void Mandelbrot::setRange_screen(const DoubleRange &range_screen_) {
    Mandelbrot::range_screen_ = range_screen_;
}

unsigned int Mandelbrot::getMaxIt() const {
    return max_it_;
}

void Mandelbrot::setMaxIt(unsigned int max_it_) {
    Mandelbrot::max_it_ = max_it_;
}

double Mandelbrot::getP() const {
    return p_;
}

void Mandelbrot::setP(double p_) {
    Mandelbrot::p_ = p_;
}
