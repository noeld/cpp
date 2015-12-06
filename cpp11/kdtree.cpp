
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iterator>
#include <stdexcept>
extern "C" {
	#include <math.h>
}

using namespace std;




class KDTree
{
public:
	using element_t = std::pair<std::vector<double>, int>;
	class KDNode
	{
	public:
		//explicit KDNode (arguments);	// type conversion
		KDNode();									// default ctor
		KDNode(const KDNode&);							// copy ctor
		KDNode(KDNode&&);								// move ctor
		KDNode& operator=(const KDNode&);				// copy assignment
		KDNode& operator=(KDNode&&);					// move assignment
		KDNode& addElement(element_t&);
		/*virtual*/ ~KDNode ();
	private:
		element_t elt_;
		KDNode* left_ { nullptr };
		KDNode* right_ { nullptr };
	};
	//explicit KDTree (arguments);	// type conversion
	KDTree();									// default ctor
	KDTree(const KDTree&);							// copy ctor
	KDTree(KDTree&&);								// move ctor
	KDTree& operator=(const KDTree&);				// copy assignment
	KDTree& operator=(KDTree&&);					// move assignment
	virtual ~KDTree ();
	void addElement(element_t&);
	void removeElement(element_t&);
	std::weak_ptr<element_t> findClosest(const element_t&) const;
	std::weak_ptr<element_t> findClosestN(const element_t&, size_t n) const;
private:
	KDNode* root_ { nullptr };
};

constexpr const char * const vectorfilename = "vectors.txt";
constexpr const char * const searchvectorfilename = "searchvectors.txt";
typedef vector<float> vector_t;

vector<unique_ptr<vector_t>> ReadFile(const char * const vectorfilename) {
	ifstream in(vectorfilename);
	vector<unique_ptr<vector_t>> vov;
	if (in.good()) {
		string tmp;
		istream_iterator<typename vector_t::value_type> iend;
		while(in.good()) {
			getline(in, tmp);
			if (0 == tmp.size())
				break;
			istringstream iss(tmp);
			istream_iterator<typename vector_t::value_type> ii(iss);
			unique_ptr<vector_t> vtemp(new vector_t);
			copy(ii, iend, back_inserter(*vtemp));
			vov.emplace_back(move(vtemp));
		}
	} else {
		cerr << "Error beim Öffnen" << endl;
	}
	return vov;
}

ostream& operator<<(ostream& out, const vector_t& v) {
	//cerr << "operator<<(ostream& out, const vector_t& v)" << endl;
	copy(v.cbegin(), v.cend(), ostream_iterator<vector_t::value_type>(cout, ", "));
	return out;
}

struct KNode
{
	KNode(size_t d) : dim(d) {}
	size_t dim;
	const vector_t* elem { nullptr };
	size_t left_index { 0 };
	size_t right_index { 0 };
	bool IsLeaf() const { return nullptr == elem || 0 == (left_index + right_index); }
	friend ostream& operator<<(ostream&, const KNode&);
};

struct KTree
{
	explicit KTree(size_t dims, size_t reserve = 0) : dims_{dims} {
		nodes_.emplace_back(0);
		if (0 < reserve)
			nodes_.reserve(reserve);
	}
	KNode& root() { return nodes_[0]; }
	size_t dims_; 
	vector<KNode> nodes_;
	KNode& add(const vector_t& v, size_t start = 0) {
		KNode& n = nodes_[start];
		//cout << "\tadd(" << v << ", start = " << start << "); (dim " << n.dim << "); ";
		if (n.elem == nullptr) {
			n.elem = &v;
			return n;
		} else {
			if (v[n.dim] < n.elem->operator[](n.dim)) {
				if (0 == n.left_index) {
					nodes_.emplace_back((n.dim + 1) % dims_);
					nodes_[start].left_index = nodes_.size() - 1;
					//cout << "Set left index to " << nodes_[start].left_index << ":" << nodes_[start] << endl;
				}
				return add(v, nodes_[start].left_index);
			} else {
				if (0 == n.right_index) {
					nodes_.emplace_back((n.dim + 1) % dims_);
					nodes_[start].right_index = nodes_.size() - 1;
					//cout << "Set right index to " << nodes_[start].right_index << ": " << nodes_[start] << endl;
				}
				return add(v, nodes_[start].right_index);
			}
		}
		//return n;
	}
	unsigned callsfnn {0};
	const KNode& FindNearestNeighbour(const vector_t& v, vector_t::value_type& distance) {
		//cout << "\tFindNearestNeighbour(" << v << ");";
		++callsfnn;
		return nodes_[FindNearestNeighbourI(v, 0, distance)];
	}
	static vector_t::value_type Distance(const vector_t& a, const vector_t& b) {
		if (a.size() != b.size()) {
			throw out_of_range("a.size() != b.size()");
		}
		vector_t::value_type d = 0;
		for(size_t i = 0; i < a.size(); ++i) {
			auto l = b[i]-a[i];
			d+=l*l;
		}
		return d;
	}
	unsigned callsfnni {0};
	double AverageSteps() const { return (callsfnn == 0)?NAN:static_cast<double>(callsfnni)/static_cast<double>(callsfnn); }
/*	size_t FindNearestNeighbourI(const vector_t& v, size_t start, vector_t::value_type& distance) {
		++callsfnni;
		KNode& n = nodes_[start];
		//cout << "FindNearestNeighbourI(" << v << ", " << start <<", " <<distance <<")" << endl;
		auto ldistance = Distance(v, *n.elem);
		if (n.IsLeaf()) {
			distance = ldistance;
			return start;
		}
		size_t next = n.right_index;
		if (v[n.dim] < n.elem->operator[](n.dim)) {
			next = n.left_index;
		}
		if (0 == next) {
			distance = ldistance;
			return start;
		}
		auto sdistance = ldistance;
		size_t sstart = FindNearestNeighbourI(v, next, sdistance);
		if (sdistance < ldistance) {
			distance = sdistance;
			return sstart;
		}
		distance = ldistance;
		return start;
	}*/
	size_t FindNearestNeighbourI(const vector_t& v, size_t start, vector_t::value_type& distance) {
		++callsfnni;
		KNode& n = nodes_[start];
		//cout << "FindNearestNeighbourI(" << v << ", " << start <<", " <<distance <<")" << endl;
		auto ldistance = Distance(v, *n.elem);
		if (n.IsLeaf()) {
			distance = ldistance;
			return start;
		}
		size_t next = n.right_index;
		if (v[n.dim] < n.elem->operator[](n.dim)) {
			next = n.left_index;
		}
		if (0 == next) {
			distance = ldistance;
			return start;
		}
		auto sdistance = ldistance;
		size_t sstart = FindNearestNeighbourI(v, next, sdistance);
		if (sdistance < ldistance) {
			vector_t vtest{*nodes_[sstart].elem};
			auto tdistance = fabs(vtest[n.dim] - (n.elem)->operator[](n.dim));
			if (sdistance >= tdistance) {
				auto sodistance = sdistance;
				auto sonext = (next == n.right_index)?n.left_index:n.right_index;
				if (0 != sonext) {
					auto sostart = FindNearestNeighbourI(v, sonext, sodistance);
					if (sodistance < sdistance) {
						distance = sodistance;
						return sostart;
					}
				}
			}
			distance = sdistance;
			return sstart;
		}
		distance = ldistance;
		return start;
	}
	friend ostream& operator<<(ostream&, const KTree&);
};

ostream& operator<<(ostream& out, const KNode& n) {
	out << "KNode(dim=" << n.dim << ";elem=";
	if (nullptr == n.elem) {
		out << "nullptr";
	} else {
		out << "(" << *n.elem << ")";
	}
	out << "left=" << n.left_index << ";right=" << n.right_index << ")";
	return out;
}
ostream& operator<<(ostream& out, const KTree& t) {
	for(size_t i = 0; i < t.nodes_.size(); ++i) {
		out << i << ": " << t.nodes_[i] << endl;
	}
	return out;
}

const vector_t& LinearScanNN(const vector<unique_ptr<vector_t>>& l, const vector_t& v, vector_t::value_type& distance) {
	auto it = l.cbegin();
	vector_t* best = it->get();
	distance = KTree::Distance(v, *it->get());
	for(++it; it != l.cend(); ++it) {
		auto ldistance = KTree::Distance(v, *it->get());
		if (ldistance < distance) {
			best = it->get();
			distance = ldistance;
		}
	}
	return *best;
}

int main (int argc, char const *argv[])
{
	try {
		vector<string> params(argv, argv+argc);
		const char* fn = (params.size() >= 2)? params[1].data() : vectorfilename;
		cout << "Lese Vektoren ...";
		cout.flush();
		vector<unique_ptr<vector_t>> vov = ReadFile(fn);
		cout << "OK\nShuffle Vektoren ...";
		cout.flush();
		random_shuffle(vov.begin(), vov.end());
		cout << "OK\nFüge Vektoren in KD-Baum ein ...";
		cout.flush();
		KTree kt(3, (params.size() >= 4)?stoul(params[3]):0);
		for(const auto& a : vov) {
			kt.add(*a);
		}
		cout << "OK" << endl;
		cout.flush();
		cout << "Lese Suchvektoren ...";
		const char* sfn = (params.size() >= 3)? params[2].data() : searchvectorfilename;
		auto fvov = ReadFile(sfn);
		cout << "OK" << endl;
		for(const auto& e : fvov) {
			cerr << "Vector (" << *e << ")";
			vector_t::value_type d = 0;
			const KNode& kn = kt.FindNearestNeighbour(*e, d);
			cout << "Nächster Vektor: " << kn << " Distanz: " << sqrt(d) << endl;
			cout << "LinearScanNN: " << LinearScanNN(vov, *e, d) << sqrt(d) << endl;
		}
		cout << "AverageSteps: " << kt.AverageSteps() << endl;
	} catch(exception& e) {
		cerr << "Exception: " << e.what() << endl;
	} catch(...) {
		cerr << "Unbekannte Exception" << endl;
	}
	return 0;
}