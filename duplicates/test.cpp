// duplicates.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "duplicates.h"


void test_for_each_adjacent_range() {
	auto bla = std::initializer_list<std::pair<int, char>>{ {1, 'A'}, {2, 'A'}, {3, 'B'}, {4, 'C'}, {5, 'C'}, {6, 'C'} };
	for_each_adjacent_range(std::begin(bla), std::end(bla)
		, [](std::pair<int, char> const& e) { return e.second; }
		, [](auto const& start, auto const& end, auto const& v) {
			std::cout << "Wert: " << v << " " << std::distance(start, end) << " Elemente.\n";
		});
}

int main(int argn, char* argv[]) {
	test_for_each_adjacent_range();
}