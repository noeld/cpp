#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
namespace b = boost;

class Variant
{
public:
	explicit Variant (const string& s) : rep_{reptype::repstring} {
		new(&asstring_) string{s};
	}	// type conversion
	explicit Variant(const int64_t& i) : rep_{reptype::repint}, asint_{i}{}
	explicit Variant(const double& d) : rep_{reptype::repdouble}, asdouble_{d}{}
	Variant() : rep_{reptype::repint}, asint_{0} {};					// default ctor
	Variant(const Variant& v) : rep_{v.rep_} {
		if (reptype::repstring == rep_) {
			new(&asstring_) string{v.asstring_};
		} else if (reptype::repint == rep_) {
			asint_ = v.asint_;
		} else {
			asdouble_ = v.asdouble_;
		}
	}							// copy ctor
	Variant(Variant&&);								// move ctor
	bool isstring() const noexcept { return reptype::repstring == rep_; }
	Variant& operator=(const Variant& v) {
		if (this != &v) {
			if (!isstring() && v.isstring()) {
				new(&asstring_) string{v.asstring_};
			} else if (isstring() && v.isstring()) {
				asstring_ = v.asstring_;
			} else if (isstring() && !v.isstring()) {
				asstring_.~string();
			}
			rep_ = v.rep_;
			if (rep_ == reptype::repint) {
				asint_ = v.asint_; 
			} else if (rep_ == reptype::repdouble) {
				asdouble_ = v.asdouble_;
			}
		}
		return *this;
	}				// copy assignment
	Variant& operator=(Variant&&);					// move assignment
	~Variant () {
		if (reptype::repstring == rep_)
			asstring_.~string();
	}

	explicit operator string() {
		switch(rep_) {
			case reptype::repint:
				{ auto tmpi = asint_;
				new(&asstring_) string{b::lexical_cast<string>(tmpi)}; }
				break;
			case reptype::repdouble:
				{ auto tmpd = asdouble_;
				new(&asstring_) string{b::lexical_cast<string>(tmpd)}; }
				break;
			case reptype::repstring:
				// nothing
				break;
		}
		rep_ = reptype::repstring;
		return asstring_;
	}
	explicit operator int64_t() {
		switch(rep_) {
			case reptype::repint:
				//nothing
				break;
			case reptype::repdouble:
				{ auto tmpd = asdouble_;
				asint_ = tmpd; }
				break;
			case reptype::repstring:
				try { 
					auto tmps =std::move(asstring_);
					asint_ = b::lexical_cast<int64_t>(tmps);
				} catch(const b::bad_lexical_cast&) {
					asint_ = 0;
				}
				asstring_.~string();
				break;
		}
		rep_ = reptype::repint;
		return asint_;
	}
	explicit operator double() {
		switch(rep_) {
			case reptype::repint:
				{ auto tmpi = asint_;
				asdouble_ = tmpi; }
				break;
			case reptype::repdouble:
				//nothing
				break;
			case reptype::repstring:
				try { 
					auto tmps =std::move(asstring_);
					asdouble_ = b::lexical_cast<double>(tmps); 
				} catch(const b::bad_lexical_cast&) {
					asdouble_ = 0.0;
				}
				asstring_.~string();
				break;
		}
		rep_ = reptype::repdouble;
		return asdouble_;
	}
private:
	enum class reptype: uint8_t { repint, repdouble, repstring } rep_;
	union {
		int64_t asint_;
		double  asdouble_;
		string  asstring_;
	};
};

int main (int argc, char const *argv[])
{
	int ret{0};
	vector<string> params(argv, argv + argc);
	Variant vi{static_cast<int64_t>(5)};
	Variant vd{5.5};
	Variant vs{string("Hallo")};
	cout << static_cast<string>(vi) << endl;
	cout << static_cast<string>(vd) << endl;
	cout << static_cast<string>(vs) << endl;
//	vs = vd;
	cout << static_cast<string>(vi) << endl;
	cout << static_cast<string>(vd) << endl;
	cout << static_cast<string>(vs) << endl;
	cout << static_cast<double>(vi) << endl;
	cout << static_cast<double>(vd) << endl;
	cout << static_cast<double>(vs) << endl;
	cout << static_cast<int64_t>(vi) << endl;
	cout << static_cast<int64_t>(vd) << endl;
	cout << static_cast<int64_t>(vs) << endl;

	return ret;
}