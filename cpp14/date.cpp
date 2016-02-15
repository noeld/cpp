//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2 -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib -o date date.cpp -lboost_system -lboost_filesystem -lboost_iostreams -lcryptopp

#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <array>
#include <boost/format.hpp>
#include <utility>

using namespace std;

class Date
{
public:
	explicit Date (unsigned yyyy, unsigned mm, unsigned dd) : val_{yyyy, mm, dd}
	{
		if (1 > yyyy || 9999 < yyyy)
			throw "year out of range";
		if (1 > mm || 12 < mm)
			throw "month out of range";
		if (1 > dd || daysOfMonth(yyyy, mm) < dd)
			throw "days ot of range";
	}	// type conversion
	explicit Date (unsigned yyyymmdd) : Date(yyyymmdd/10000, (yyyymmdd/100)%100, yyyymmdd%100) 
	{}
	Date() =default;									// default ctor
	Date(const Date& d) : val_{d.val_.yyyy, d.val_.mm, d.val_.dd}{}							// copy ctor
	//Date(Date&&);								// move ctor
	Date& operator=(const Date& d) {
		if (this != &d) {
			val_.yyyy = d.val_.yyyy;
			val_.mm = d.val_.mm;
			val_.dd = d.val_.dd;
		}
		return *this;
	}				// copy assignment
	//Date& operator=(Date&&);					// move assignment
	~Date ()
	{}
	
	constexpr unsigned year() const noexcept { return val_.yyyy; }
	constexpr unsigned month() const noexcept { return val_.mm; }
	constexpr unsigned day() const noexcept { return val_.dd; }
	constexpr unsigned yyyymmdd() const noexcept { return year()*10000 + month() * 100 + day(); }
	explicit constexpr operator unsigned() const noexcept { return yyyymmdd(); }
	
	Date firstOfYear() const noexcept { return Date(year(), 1, 1); }
	Date lastOfYear() const noexcept { return Date(year(), 12, 31); }
	Date firstOfMonth() const noexcept { return Date(year(), month(), 1); }
	Date lastOfMonth() const noexcept { return Date(year(), month(), Date::daysOfMonth(year(), month())); }
	
	bool isLeapYear() const noexcept { return Date::isLeapYear(year()); }
	unsigned dayOfYear() const noexcept {
		auto d = Date::doma_[month()-1] + day();
		if (month() > 2 && isLeapYear())
			++d;
		return d;
	}
	
	unsigned weekday() const noexcept {
		// http://www.timeanddate.com/date/doomsday-weekday.html
		int a1 = (year() % 100)/12;
		int a2 = (year() % 100) - (12 * a1);
		int a3 = a2/4;
		int a4 = Date::anchor_[(year()/100)%4];
		int a5 = a1 + a2 + a3 + a4;
		int a6 = a5 % 7;
		auto zz = a6 + static_cast<int>(dayOfYear()) - 31 - static_cast<int>(Date::daysOfMonth(year(), 2));
		auto z = (zz) %7;
		if (zz < 0)
			z = (7 + z)%7;
		//cout << "(" << zz << ", " << z << ")";
		auto wt = std::array<unsigned, 7>{{7, 1, 2, 3, 4, 5, 6}}[z];
		return wt;
	}
	unsigned weekOfYear() const noexcept {
		auto wfoy = firstOfYear().weekday()-1;
		auto doy = dayOfYear();
		auto firstweeknr = (wfoy > 0 && wfoy < 5)?1:0;
		auto nw = (doy / 7) + firstweeknr;
		return nw;
	}
	
	
	static bool isLeapYear(unsigned yyyy) noexcept {
		return (yyyy % 4 == 0) && ((yyyy % 100 != 0) || (yyyy % 400 == 0)); 
	}
	static unsigned daysOfYear(unsigned yyyy) noexcept {
		return isLeapYear(yyyy)?366:365;
	}
	static unsigned daysOfMonth(unsigned yyyy, unsigned mm) {
		auto d = dom_[mm];
		if (2 == mm && isLeapYear(yyyy)) {
			d += 1;
		}
		return d;
	}
	static unsigned daysSinceEpoch(unsigned yyyy, unsigned mm, unsigned dd) {
		static constexpr auto d400y = 365 * 400 + 100 - 4 + 1;
		auto ys400 = yyyy / 400 * d400y;
		auto yyy = yyyy % 400 + 1;
	}
	static const char* dayName(unsigned d) {
		if (d < 1 || d > 7)
			throw "Weekday out of range";
		return days_[d];
	}
	Date& operator++() {
		auto d = day() + 1;
		if (d > Date::daysOfMonth(year(), month())) {
			++val_.mm;
			val_.dd = 1;
			if (13 == val_.mm) {
				++val_.yyyy;
				if (9999 < val_.yyyy)
					throw "date is out of range";
				val_.mm = 1;
			}
		} else {
			val_.dd = d;
		}
		return *this;
	}
	Date operator++(int) {
		Date tmp(*this);
		this->operator++();
		return tmp;
	}
	Date& operator--() {
		auto d = day() - 1;
		if (0 == d) {
			auto m = month() - 1;
			if (0 == m) {
				m = 12;
				auto y = year() - 1;
				if (0 == y)
					throw "year is out of range";
				val_.yyyy = y;
			}
			val_.mm = m;
			d = Date::daysOfMonth(year(), month());
		}
		val_.dd = d;
		return *this;
	}
	Date operator--(int) {
		Date tmp(*this);
		this->operator--();
		return tmp;
	}
	constexpr bool operator<(const Date& d) const noexcept {
		return yyyymmdd() < d.yyyymmdd(); 
	}
	constexpr bool operator==(const Date& d) const noexcept {
		return !((*this < d) && (d < *this));
	}
	constexpr bool operator>(const Date& d) const noexcept {
		return d < *this;
	}
	constexpr bool operator<=(const Date& d) const noexcept {
		return !(d < *this);
	}
	constexpr bool operator>=(const Date& d) const noexcept {
		return !(*this < d);
	}
private:
	static const std::array<unsigned, 13> dom_;
	static const std::array<unsigned, 12> doma_;
	static const std::array<unsigned, 4> anchor_;
	static const std::array<const char*, 8> days_;
	struct ct {
		unsigned yyyy : 14;
		unsigned mm   :  4;
		unsigned dd   :  5;
	} val_ {1,1,1};
//	friend void std::swap(Date&, Date&);
};

constexpr const std::array<unsigned, 13> Date::dom_  = {{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};
constexpr const std::array<unsigned, 12> Date::doma_ = {{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }};
constexpr const std::array<unsigned, 4> Date::anchor_ = {{ 2, 6, 5, 3 }};
constexpr const std::array<const char*, 8> Date::days_ = {{ "", "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" }};

ostream& operator<<(ostream& o, const Date& d) {
	return o << boost::format{"%04d-%02d-%02d"} % d.year() % d.month() % d.day();
}

int main (int argc, char const *argv[])
{
	int ret {0};
	cout << sizeof(Date) << endl;
	Date d;
	cout << d << endl;
	Date d2(9999,12,31); cout << d2 << endl;
	Date d3(20161231); cout << d3 << endl;
	/*
	for (unsigned y = 1999; y <= 2020; ++y) {
			cout << "Date::isLeapYear(" << y << "): " << boolalpha << Date::isLeapYear(y) << endl;
	}
	*/
	/*
	for(auto i = 1, n = 0; i < 13; ++i, n += Date::dom_[i-1] )
		cout << i << ": " << n << endl;
	*/
	for(Date d(2016,2,28), de(2015,12,25); d > de; --d) {
		cout << d << " - " << boost::format{"%3d - %d - %s - KW%02s"} % d.dayOfYear() % d.weekday() % Date::dayName(d.weekday()) % d.weekOfYear() << endl;
	}
	/*
	for(Date d(1,1,1), e(9999,12,31); d<e; ++d) {
		Date tmp(d.year(), d.month(), d.day());
		if (tmp.month() == 1 and tmp.day() == 1) {
			cout << tmp.year() << " ";
		}
	}
	cout << endl;
	for(Date d(9999,12,31), e(1,1,1); d>e; --d) {
		Date tmp(d.year(), d.month(), d.day());
		if (tmp.month() == 1 and tmp.day() == 1) {
			cout << tmp.year() << " ";
		}
	}
	cout << endl;
	*/
	return ret;
}