// https://www.codechef.com/problems/UNIVERSE

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <random>
#include <atomic>
#include <algorithm>
#include <memory>
#include <limits>
#include <queue>

//////////////////////////////////////
template<typename T> 
struct Counted {
	int Id() const { return id_; }
	const int id_ { NextCounter() };
	static std::atomic_int counter;
	static int NextCounter() { return ++counter; }
	static int Counter() { return counter; }
};
template<typename T> std::atomic_int Counted<T>::counter {0};

struct Planet : public Counted<Planet> {
	
};
struct UniverseProto {
	explicit UniverseProto(int size) : size_{size} {}
	const int size_;
	static std::unique_ptr<UniverseProto> instance;
};
struct Universe : public Counted<Universe> {
	
};
struct Teleport {

};
struct Multiverse {
	const int size_;
};

//////////////////////////////////////

using node_t = unsigned;

class NodeList;

template<typename T>
struct Range {
	using const_iterator_t = typename T::const_iterator;
	Range(const_iterator_t begin, const_iterator_t end)
	: begin_(begin), end_(end) {}
	Range(const std::pair<const_iterator_t, const_iterator_t>& p)
	: Range(p.first, p.second) {}
	~Range() = default;
	Range(const Range&) = default;
	Range& operator=(const Range&) = default;
	const const_iterator_t& begin() const { return begin_; }
	const const_iterator_t& end()   const { return end_; }
	const const_iterator_t begin_, end_;
};

class EdgeList {
public:
	using value_type  = std::pair<node_t, node_t>;
	using list_t      = std::set<value_type>;
	// using edge_list_t = std::vector<value_type>;
	using node_list_t = std::vector<node_t>;
	using Range	= ::Range<list_t>;
	void AddEdge(node_t from, node_t to) {
		list_.insert(std::make_pair(from, to));
	}
	void AddBidirectionalEdge(node_t from, node_t to) {
		list_.insert(std::make_pair(from, to));
		list_.insert(std::make_pair(to, from));
	}
	Range GetEdges(node_t node) const {
		auto keybegin = std::make_pair    ( node     , node_t ( ) ) ;
		auto keyend   = std::make_pair    ( node + 1 , node_t ( ) ) ;
		auto from     = list_.lower_bound ( keybegin ) ;
		auto to       = list_.upper_bound ( keyend   ) ;
		return Range(from, to);
	}
	bool HasEdge(node_t from, node_t to) const {
		return list_.find(std::make_pair(from, to)) != list_.end();
	}
	node_list_t ShortestPath(const node_t& start, const node_t& dst) const {
		// https://de.wikipedia.org/wiki/Dijkstra-Algorithmus
		using dist_t = unsigned;
		constexpr const dist_t INF = std::numeric_limits<dist_t>::max();
		using status_t = struct { 
			dist_t distance    { INF   } ;
			node_t predecessor { 0     } ;
			bool   visited     { false } ;
		};
		node_t max_id = [this]() -> node_t {
			auto r = list_.rbegin();
			if (r == list_.rend())
				return 0;
			return r->first;
		}();
		// initialize
		auto status = std::make_unique<status_t[]>(max_id+1);
		status[start].distance = 0;
		auto comp = [&status](const node_t& a, const node_t& b) { return status[a].distance < status[b].distance; };
		std::priority_queue<node_t, std::vector<node_t>, decltype(comp)> q(comp, std::vector<node_t>());
		q.emplace(start);
		node_t node = 0;
		while(!q.empty()) {
			node = q.top();
			q.pop();
			if (node == dst) 
				break;
			for(const auto& e : GetEdges(node)) {
				dist_t alt = status[node].distance + 1;
				auto& st =  status[e.second];
				if (alt < st.distance) {
					st.distance    = alt;
					st.predecessor = node;
					if (!st.visited) {
						st.visited = true;
						q.emplace(e.second);
					}
				}
			}
		}
		node_list_t el;
		if (node == dst) {
			el.reserve(status[node].distance);
			while(status[node].predecessor != 0) {
				el.push_back(node);
				node = status[node].predecessor;
			}
			el.push_back(start);
			std::reverse(el.begin(), el.end());
		}
		return el;
	}
	friend std::ostream& operator<<(std::ostream& o, const EdgeList& e) {
		auto it = e.list_.begin();
		auto end = e.list_.end();
		if(it != end)
			o << "[" << it->first << "; " << it->second << "]";
		++it;
		for(; it != end; ++it) {
			o << ", [" << it->first << "; " << it->second << "]";
		}
		return o;
	}
private:
	
	// using index_t = std::vector<list_t::const_iterator>>;
	list_t list_;
	// index_t index_;
};

class Graph {
public:
	using node_t = unsigned;
	using node_list_t = std::set<node_t>;
	using edge_list_t = std::vector<std::pair<node_t, node_t>>;

	bool AddEdge(const node_t& from, const node_t& to);
	bool HasEdge(const node_t& from, const node_t& to);
	edge_list_t Edges(const node_t& node);
	node_list_t Neighbours(const node_t& node);
	node_list_t ShortestPath(const node_t& from, const node_t& to);
private:
	edge_list_t edge_list_;
};





int main(int argc, char const *argv[])
{
	using node_t = Graph::node_t;
	EdgeList el;
	std::random_device rd;
	std::uniform_int_distribution<Graph::node_t> id(1, 20);
	auto next_num = [&rd, &id]() -> Graph::node_t { return id(rd); };
	const size_t N = 20;
	for(auto i = 0; i < N; ++i) {
		auto n1 = next_num();
		auto n2 = next_num();
		while(n2 == n1)
			n2 = next_num();
		el.AddBidirectionalEdge(n1, n2);
	}
	std::cout << el << std::endl;
	std::cout << "Neighbours of: ";
	node_t src;
	std::cin >> src;
	for(auto e : el.GetEdges(src)) {
		std::cout << e.first << ", " << e.second << std::endl;
	}
	std::cout << "Shortest Path to; ";
	node_t dst;
	std::cin >> dst;
	auto path = el.ShortestPath(src, dst);
	for(auto e : path) {
		std::cout << e << std::endl;
	}
	return 0;
}