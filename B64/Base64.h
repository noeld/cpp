//
// Created by Arnold Müller on 07.05.17.
//

#ifndef B64_BASE64_H
#define B64_BASE64_H


#include <array>

class Base64 {
public:
    union conversion_t {
        char     cc[4];
        uint32_t cu;
    };
    union rconversion_t {
        unsigned char cc[4];
        uint32_t cu;
    };
    static const std::array<char, 64> alpha_;
    static const std::array<unsigned char, 0x100> ralpha_;
    static const char RALPHA_UNDEFINED;
    static const char RALPHA_FILL;

    static uint32_t EncodeByteTriplet(const unsigned char *c);
    static std::string Encode(const std::string&);
    //static void BuildReverseAlpha(const decltype(Base64::alpha_) &, std::array<char, 0x80> &);
    static uint32_t DecodeCharQuadruplet(const char *c);
    static std::string Decode(const std::string&);
};


#endif //B64_BASE64_H
