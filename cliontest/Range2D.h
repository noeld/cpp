//
// Created by Arnold Müller on 10.02.17.
//

#ifndef CLIONTEST_RANGE2D_H
#define CLIONTEST_RANGE2D_H


#include <algorithm>
#include <ostream>
#include <boost/format.hpp>

template < class value_type >
class Range2D {
public:
    Range2D(const value_type& xmin
            , const value_type& ymin
            , const value_type& xmax
            , const value_type& ymax )
    : xmin_(xmin), ymin_(ymin), xmax_(xmax), ymax_(ymax)
    {
        if (xmin_ > xmax_)
            std::swap(xmin_, xmax_);
        if (ymin_ > ymax_)
            std::swap(ymin_, ymax_);
    }

    Range2D() : Range2D(-2.5, -1.5, 1.0, 1.5) {}
    Range2D(const Range2D&) = default;
    Range2D(Range2D&&) = default;
    ~Range2D() = default;
    Range2D& operator=(const Range2D&) = default;
    Range2D& operator=(Range2D&&) = default;

    value_type width() const { return xmax_ - xmin_ ;}
    value_type height() const { return ymax_ - ymin_ ;}


    Range2D& fit (value_type resx, value_type resy, double rel = 0.5) {
        auto relres = resx * rel / resy;
        auto relrange = width() / height();
        if (relrange < relres) {
            // height() / resy = ? / resx; ? = height() * resx / resy;
            auto wanted_width = height() * resx * rel / resy;
            auto delta_width = wanted_width - width();
            xmin_ -= delta_width / 2.0;
            xmax_ += delta_width / 2.0;
        } else if (relrange > relres) {
            // width() / resx = ? / resy; ? = width() * resy / resx;
            auto wanted_height = width() * resy / (resx * rel);
            auto delta_height = wanted_height - height();
            ymin_ -= delta_height / 2.0;
            ymax_ += delta_height / 2.0;
        }
        return *this;
    }

    Range2D& fit (const Range2D<value_type>& r) {
        return fit(r.width(), r.height());
    }

    Range2D fit_copy(const Range2D& r) const {
        Range2D tmp(*this);
        tmp.fit(r);
        return tmp;
    }

    Range2D& scale(value_type f) {
        auto new_width_add = width() * ( 1 - f) / 2.0 ;
        auto new_height_add = height() * (1 - f) / 2.0;
        xmin_ += new_width_add;
        xmax_ -= new_width_add;
        ymin_ += new_height_add;
        ymax_ -= new_height_add;
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const Range2D &d) {
        os << "xmin_: " << d.xmin_ << " ymin_: " << d.ymin_ << " xmax_: " << d.xmax_ << " ymax_: " << d.ymax_;
        os << boost::format{"(%6.3f, %6.3f)"} % d.width() % d.height();
        //os << " (" << d.width() << ", " << d.height() << ")";
        return os;
    }

    value_type xmin_, ymin_, xmax_, ymax_;
};



using DoubleRange = Range2D<double>;
using FloatRange = Range2D<float>;

#endif //CLIONTEST_RANGE2D_H
