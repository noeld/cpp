// g++ -std=c++11 mandel2.cpp -o mandel2 -O3 -lpng

#include <iostream>
#include <array>
#include <complex>
#include <utility>
#include <tuple>
#include <sstream>

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/format.hpp> 

class FrAdapter;

class Fr
{
public:
	friend class FrAdapter;

	using fl_t = double;
	using cp_t = std::complex<fl_t>;

	Fr() = default;
	Fr(const Fr&) = default;
	~Fr() = default;

	std::pair<fl_t, fl_t> step() const {
		return std::make_pair(
			((xmax_ - xmin_) / res_x_)
			, ((ymax_ - ymin_) / res_y_)
			);
	}

	char project_iter(unsigned i) const {
		return ccc_[i * ccc_.size() / static_cast<fl_t>(max_)];
	}

	unsigned iterate(const cp_t& c) const {
		cp_t cc = c;
		unsigned i = 0;
		while(i < max_) {
			if (abs(cc) > 4.0)
				return i;
			cc = pow(cc, this->pow_) + c;
			++i;
		}
		return max_ - 1;
	}

	fl_t iterate_d(const cp_t& c) const {
		return this->iterate(c) / static_cast<fl_t>(max_);
	}

	void render(std::ostream& o = std::cout) const {
		fl_t fx, fy;
		std::tie(fx, fy) = this->step();
		for(unsigned ry = 0; ry < res_y_; ++ry) {
			fl_t y = ymax_ - ry * fy;
			for(unsigned rx = 0; rx < res_x_; ++rx) {
				unsigned i = this->iterate(cp_t(xmin_ + rx * fx, y));
				o << this->project_iter(i);
			}
			o << std::endl;
		}
	} 

	void fit_range_to_res(fl_t fd = 0.5) {
		//std::pair<fl_t, fl_t> old = std::make_pair(res_x_, res_y_);
		fl_t fr = static_cast<fl_t>(res_x_) / static_cast<fl_t>(res_y_) * fd;
		//std::cout << "fr " << fr << std::endl;
		auto lx = (xmax_ - xmin_);
		auto ly = (ymax_ - ymin_);
		fl_t fw = lx / ly;
		//std::cout << "fw " << fw << std::endl;
		if (fr > fw) {
			// stretch x
			auto lxn = ly * fr;
			auto lxd = lxn - lx;
			xmin_ -= lxd / 2.0;
			xmax_ += lxd / 2.0;
		} else if (fr < fw) {
			// stretch y
			// fr = 5/4 = 1.25
			// fw = 3/2 = 1.5
			// fwn= 3/
			auto lyn = lx * 1 / fr;
			auto lyd = lyn - ly;
			ymin_ -= lyd / 2.0;
			ymax_ += lyd / 2.0;
		}
	}

	void set_res(unsigned rx, unsigned ry) {
		std::tie(this->res_x_, this->res_y_) = std::make_tuple(rx, ry);
	}

	void set_pow(fl_t p) {
		this->pow_ = p;
	}

	void set_max(unsigned m) {
		this->max_ = m;
	}

	void set_window(const std::tuple<fl_t, fl_t, fl_t, fl_t>& w) {
		std::tie(xmin_, ymin_, xmax_, ymax_) = w;
		if (xmin_ > xmax_)
			std::swap(xmin_, xmax_);
		if (ymin_ > ymax_)
			std::swap(ymin_, ymax_);
	}

private:
	fl_t xmin_{-2.0}, ymin_{-1.2}, xmax_{0.8}, ymax_{1.2};
	fl_t pow_{2.0};
	unsigned max_ {256};
	unsigned res_x_ {1000}, res_y_{500};
	std::array<char, 14> ccc_ {{'-', '.', ',', ':', ';', '+', '*', '=', 'o', 'O', '0', '#', 'M', ' '}};
};

class FrAdapter {
public:
	typedef boost::gil::point2<ptrdiff_t>   point_t;

    typedef FrAdapter           const_t;
    typedef boost::gil::gray8_pixel_t       value_type;
    typedef value_type          reference;
    typedef value_type          const_reference;
    typedef point_t             argument_type;
    typedef reference           result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    FrAdapter(const point_t& siz = point_t(200,76), Fr::fl_t pow = 2.0) {
    	fr_.set_res(siz.x, siz.y);
    	fr_.fit_range_to_res(1.0);
    	fr_.set_pow(pow);
    }

    Fr::cp_t map_to_complex(const point_t& p) const {
    	return Fr::cp_t(((p.x / static_cast<Fr::fl_t>(fr_.res_x_)) * (fr_.xmax_ - fr_.xmin_)) + fr_.xmin_
    			      , ((p.y / static_cast<Fr::fl_t>(fr_.res_y_)) * (fr_.ymax_ - fr_.ymin_)) + fr_.ymin_
    		);
    }
    result_type operator()(const point_t& p) const {
    	auto c = this->map_to_complex(p);
    	//std::cerr << fr_.xmax_ - fr_.xmin_ << " ";
    	//std::cerr << "(" << p.x << "; " << p.y << ") -> " << c << std::endl; //<< c.real() << ", " << c.imag() << ")" << std::endl;
    	auto i = fr_.iterate_d(c);
    	return value_type((boost::gil::bits8)(pow(i,this->pow_)*255));
    }

    Fr& fr() {return fr_;};

    void set_pow(Fr::fl_t p) {
    	this->pow_ = p;
    }

private:
	Fr fr_;
	Fr::fl_t pow_{0.5};
};

int main(int argc, char const *argv[])
{
	//Fr f;
	//std::cout << f.step().first << " " << f.step().second << std::endl;
	//f.render();
	//f.set_res(200, 76);
	//f.set_res(400, 150);
	//f.set_res(800, 300);
	//f.set_res(4000, 1600);
	//f.render();
	//f.fit_range_to_res();
	//std::cout << f.step().first << " " << f.step().second << std::endl;
	//f.render();
	typedef boost::gil::virtual_2d_locator<FrAdapter,false> locator_t;
	typedef boost::gil::image_view<locator_t> my_virt_view_t;
	FrAdapter::point_t res(1000,1000);

	for(double d = 2.0; d < 2.1; d += 0.025) {
		for(double p = 0.5; p < 0.6; p += 0.1) {
			std::ostringstream oss;
			oss << boost::format{"p_%3.2f_%2.1f.png"} % d % p;
			std::string filename = oss.str();
			std::cout << filename << std::endl; 
			FrAdapter fra(res, d);
			//fra.fr().set_window(std::make_tuple(-2.0, -1.5, 1.0, 1.5));
			fra.fr().set_window(std::make_tuple(-0.0, -0.9, 0.4, -0.4));
			fra.fr().fit_range_to_res(1.0);
			fra.set_pow(p);
			my_virt_view_t mandel(res, locator_t(FrAdapter::point_t(0, 0), FrAdapter::point_t(1, 1), fra));

			//boost::gil::gray8s_image_t img(res);
			boost::gil::png_write_view(filename, mandel);
	
		}
	}

	return 0;
}