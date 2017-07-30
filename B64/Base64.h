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
    template<typename T, typename I> static void EncodeIt(T start, const T end, I in);
    //static void BuildReverseAlpha(const decltype(Base64::alpha_) &, std::array<char, 0x80> &);
    static uint32_t DecodeCharQuadruplet(const char *c);
    static std::string Decode(const std::string&);
};


template<typename T, typename I>
void Base64::EncodeIt(T start, const T end, I in) {
    char c[3];
    size_t i = 0;
    while(start != end) {
        c[i % 3] = *start;
        ++start;
        if (++i % 3 == 0) {
            //conversion_t t;
            //t.cu = 0;
            conversion_t t2;
            t2.cu = EncodeByteTriplet(reinterpret_cast<unsigned char*>(&c[0]));
            *in = (t2.cc[0]);
            *in = (t2.cc[1]);
            *in = (t2.cc[2]);
            *in = (t2.cc[3]);
        }
    }
    if (i % 3 != 0) {
        auto missing = 3 - (i % 3);
        for (auto n = i % 3; n < 3; ++n) {
            c[n] = 0;
        }
        //conversion_t t;
        //t.cu = 0;
        conversion_t t2;
        t2.cu = EncodeByteTriplet(reinterpret_cast<unsigned char*>(&c[0]));
        *in = (t2.cc[0]);
        *in = (t2.cc[1]);
        *in = ( (missing >= 2) ? '=' : t2.cc[2]);
        *in = ('=');
    }
}

#endif //B64_BASE64_H
