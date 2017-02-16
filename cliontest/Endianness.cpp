//
// Created by Arnold Müller on 10.02.17.
//

#include "Endianness.h"

//constexpr const int Endianness::i{ 0x01 };

bool Endianness::IsLittleEndian() {
    constexpr const int i = 0x01;
    return 0x01 == *reinterpret_cast<const char*>(&i);
}

