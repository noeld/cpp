// duplicates.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <iostream>

// TODO: Verweisen Sie hier auf zusätzliche Header, die Ihr Programm erfordert.

/**
* Find ranges (neighbouring elements) of the same value within [begin, end[ and
* call callback for each such range
* @param begin start of container
* @param end end of container (1 beyond last element)
* @param function returns value for each iterator V(*T&)
* @param callback void(start, end, value)
* @return number of range
*/
template<typename  T, typename V, typename F>
size_t for_each_adjacent_range(T begin, T end, V getvalue, F callback) {
	size_t partitions = 0;
	while (begin != end) {
		auto const& value = getvalue(*begin);
		auto current = begin;
		while (++current != end && getvalue(*current) == value);
		callback(begin, current, value);
		++partitions;
		begin = current;
	}
	return partitions;
}

