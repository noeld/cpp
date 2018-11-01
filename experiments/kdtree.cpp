#include <iostream>
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include <stack>
#include <memory>
#include <tuple>

using coord_t = double; 

template<size_t DIM = 3>
class Point {
public:
	using member_t = coord_t[DIM];
	Point() = default;
	Point(const std::initializer_list<coord_t>& l) {
		std::copy(l.begin(), l.end(), this->begin());
	}
    Point(const Point& p) {
    	std::copy(p.begin(), p.end(), this->begin());
    }
	// explicit Point(coord_t ...) : parent_t(...) {}
	Point operator-(const Point& p) const {
		Point tmp;
		for(size_t i = 0; i < DIM; ++i)
			tmp[i] = this->operator[](i) - p[i];
		return tmp;
	}
	coord_t& operator[](const size_t& i) noexcept { return array_[i]; }
	const coord_t& operator[](const size_t& i) const noexcept { return array_[i]; }
	coord_t Norm() const { return TMAdd<DIM-1>(); }
	coord_t* begin() { return &array_[0]; }
	const coord_t* begin() const { return &array_[0]; }
	const coord_t* end() const { return &array_[DIM]; }
	coord_t Length() const {
		return std::sqrt(Norm());
	}
	coord_t Distance(const Point& p) const {
		return (*this - p).Length();
	}
	// bool operator==(const Point& p) const {
	// 	return 
	// }
	// template<typename op, typename op2, size_t D>
	// decltype(auto) TMEvaluate(const Point& p) const {
	// 	return op2(op(array_[D], p.array_[D]), TMEvaluate<op, op2, D-1>(p));
	// }
	// template<> decltype(auto) TMEvaluate<std::equal_to, std::logical_and, 0>(const Point& p) const {
	// 	return std::equal_to<void>(array_[0], p.array_[0]);
	// }

	// bool operator==(const Point& p) const {
	// 	return TMEvaluate<std::equal_to, std::logical_and, DIM-1>(p);
	// }
private:
	member_t array_;
	template<size_t D> coord_t TMAdd() const { return (this->operator[](D) * this->operator[](D)) + TMAdd<D-1>(); }
};

template<size_t DIM> template<> coord_t Point<DIM>::TMAdd<0>() const { return (*this)[0] * (*this)[0] ;}

template<size_t DIM = 3>
struct Node
{
	Point<DIM> p_;
	std::unique_ptr<Node> left_, right_;
	template<typename T>
	void MakeTree(typename T::iterator& start, typename T::iterator& end, size_t dim) {
		auto compare = [&dim](typename T::iterator& a, typename T::iterator& b) {
			return (*a)[dim] < (*b)[dim];
		};
		auto middle = std::nth_element(start, start + (end-start)/2, end, compare);
		this->p_ = *middle;
		if (middle - start > 0) {
			this->left_ = std::make_unique<Node>();
			this->left_->MakeTree(start, middle, (dim + 1) % DIM);
		}
		if (end - middle > 0) {
			this->right_ = std::make_unique<Node>();
			this->right_->MakeTree(middle, end, (dim + 1) % DIM);
		}
	}
};

template<size_t DIM>
class KDTree
{
public:
	using value_type = Node<DIM>;
	KDTree();
	~KDTree();
	template<typename T>
	void MakeTree(typename T::iterator& start, typename T::iterator& end) {
		root_ = std::make_unique<value_type>();
		root_->MakeTree(start, end, 0);
	}
	static constexpr size_t Dim() noexcept { return DIM; }
private:
	std::unique_ptr<value_type> root_;
};

// template<typename T>
// typename T::iterator Quickselect(typename T::iterator start, typename T::iterator end, size_t k) {
// 	while(1) {
// 		if (start + 1 == end)
// 			return start;
// 		auto pivot = start + (end - start) / 2;
// 		auto middle = std::partition(start, end, [pivot](const typename T::value_type& e) { return e < *pivot; });
// 		auto d = middle - start - 1;
// 		if (d == k)
// 			return middle;
// 		else if (k < d) 
// 			end = pivot;
// 		else start = pivot;
// 	}
// }

int main(int argc, char const *argv[])
{
	using namespace std;
	int ret = 0;
	Point<3> a = {1, 1, 0};
	Point<3> b = {0, 0, 0};
	cout << a.Distance(b) << endl;
	cout << a.Norm() << endl << endl;

	// std::vector<int> bsp { 1 , 2, 3 , 4, 5, 6};
	// std::random_shuffle(bsp.begin(), bsp.end());
	// for(int i = 0; i < bsp.size(); ++i) {
	// 	// auto it = Quickselect<std::vector<int>>(bsp.begin(), bsp.end(), i);
	// 	std::nth_element(bsp.begin(), bsp.begin() + i, bsp.end());
	// 	cout << i << ". Kleinstes Element: " << bsp[i] << endl;
	// }

	std::vector<Node<3>> v = {
		{{2, 3}}, {{5, 4}}, {{9, 6}}, {{4, 7}}, {{8, 1}}, {{7, 2}}
	};
	return ret;
}