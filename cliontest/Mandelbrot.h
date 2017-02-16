//
// Created by Arnold Müller on 10.02.17.
//

#ifndef CLIONTEST_MANDELBROT_H
#define CLIONTEST_MANDELBROT_H


#include <iosfwd>
#include <iostream>
#include <vector>
#include "Range2D.h"

class Mandelbrot {
public:
    Mandelbrot();
    Mandelbrot(const DoubleRange& window, const DoubleRange& screen, std::ostream& out);
    ~Mandelbrot() = default;
    void run() const;
    static unsigned int iter(const double& x, const double& y, unsigned int maxit, const double& p);
private:
    char alphaFor(unsigned i) const {
        return alpha_[static_cast<double>(i)/max_it_ * alpha_.size()];
    }

    std::ostream& out_ { std::cout };
    std::vector<char> alpha_ {'_', '.', ':', ';', 'x', 'X', 'M', '-'};
    DoubleRange range_window_ { -2.5, -1.5, 1, 1.5 };
    DoubleRange range_screen_ { 0, 0, 80, 40 };
    unsigned max_it_ { 200 };
    double p_ { 2.0 };
public:
    const DoubleRange &getRange_window() const;

    void setRange_window(const DoubleRange &range_window_);

    const DoubleRange &getRange_screen() const;

    void setRange_screen(const DoubleRange &range_screen_);

    unsigned int getMaxIt() const;

    void setMaxIt(unsigned int max_it_);

    double getP() const;

    void setP(double p_);
};


#endif //CLIONTEST_MANDELBROT_H
