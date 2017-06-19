//
// Created by Arnold Müller on 07.05.17.
//

#include <string>
#include "Base64.h"

constexpr const std::array<char, 64> Base64::alpha_ {
        //"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,  0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,  0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,  0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
        0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33,  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2b, 0x2f
};

constexpr const char Base64::RALPHA_UNDEFINED = std::numeric_limits<char>::max();
constexpr const char Base64::RALPHA_FILL      = std::numeric_limits<char>::max() - 1;

constexpr const std::array<unsigned char, 0x100> Base64::ralpha_ {
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x3e, 0x7f, 0x7f, 0x7f, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,  0x3c, 0x3d, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f,
        0x7f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
        0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,  0x17, 0x18, 0x19, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,  0x31, 0x32, 0x33, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,

        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
        0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f
};

uint32_t Base64::EncodeByteTriplet(const unsigned char *c) {
    conversion_t t;
    t.cc[0] = alpha_[         c[0] >> 2                 ];
    t.cc[1] = alpha_[ 077 & ((c[0] << 4) | (c[1] >> 4)) ];
    t.cc[2] = alpha_[ 077 & ((c[1] << 2) | (c[2] >> 6)) ];
    t.cc[3] = alpha_[ 077 & (               c[2]      ) ];
    return t.cu;
}

uint32_t Base64::DecodeCharQuadruplet(const char *c) {
    rconversion_t t;
    static const char* err = "Base64 character out of range";
    unsigned char i[4];
    i[0] = Base64::ralpha_[static_cast<size_t>(c[0])];
    i[1] = Base64::ralpha_[static_cast<size_t>(c[1])];
    i[2] = Base64::ralpha_[static_cast<size_t>(c[2])];
    i[3] = Base64::ralpha_[static_cast<size_t>(c[3])];
    if (i[0] == RALPHA_UNDEFINED || i[0] == RALPHA_FILL)
        throw "Not a valid base 64 character 0";
    if (i[1] == RALPHA_UNDEFINED || i[1] == RALPHA_FILL)
        throw "Not a valid base 64 character 1";
    if (i[2] == RALPHA_UNDEFINED)
        throw "Not a valid base 64 character 2";
    if (i[2] == RALPHA_FILL)
        i[2] = 0;
    if (i[3] == RALPHA_UNDEFINED)
        throw "Not a valid base 64 character 3";
    if (i[3] == RALPHA_FILL)
        i[3] = 0;
    t.cc[0] = i[0] << 2 | i[1] >> 4;
    t.cc[1] = i[1] << 4 | i[2] >> 2;
    t.cc[2] = i[2] << 6 | i[3] ;
    t.cc[3] = 0;
    return t.cu;
}

std::string Base64::Encode(const std::string &s) {
    auto b = std::begin(s);
    auto e = std::end(s);
    std::string ret;
    char c[3];
    size_t i = 0;
    while(b != e) {
        c[i % 3] = *b;
        ++b;
        if (++i % 3 == 0) {
            //conversion_t t;
            //t.cu = 0;
            conversion_t t2;
            t2.cu = EncodeByteTriplet(reinterpret_cast<unsigned char*>(&c[0]));
            ret.push_back(t2.cc[0]);
            ret.push_back(t2.cc[1]);
            ret.push_back(t2.cc[2]);
            ret.push_back(t2.cc[3]);
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
        ret.push_back(t2.cc[0]);
        ret.push_back(t2.cc[1]);
        ret.push_back( (missing >= 2) ? '=' : t2.cc[2]);
        ret.push_back('=');
    }
    return ret;
}

std::string Base64::Decode(const std::string &s) {
    auto b = s.begin();
    auto e = s.end();
    std::string ret;
    char c[4];
    size_t i = 0;
    while(b != e) {
        c[i % 4] = *b;
        ++b;
        if (++i % 4 == 0 || b == e) {
            rconversion_t t;
            t.cu = Base64::DecodeCharQuadruplet(c);
            ret.push_back(t.cc[0]);
            if (c[2] != '=')
                ret.push_back(t.cc[1]);
            if (c[3] != '=')
                ret.push_back(t.cc[2]);
        }
    }
    return ret;
}

template<typename T, typename I>
void Base64::EncodeIt(T &start, const T &end, I &in) {
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

//void Base64::BuildReverseAlpha(const decltype(Base64::alpha_) &alpha, std::array<char, 0x80> &ralpha) {
//    std::fill(ralpha.begin(), ralpha.end(), RALPHA_UNDEFINED);
//    {
//        size_t i = 0;
//        for(auto&& c : alpha) {
//            ralpha[static_cast<char>(c)] = i;
//            ++i;
//        }
//    }
//    ralpha[static_cast<char>('=')] = RALPHA_FILL;
//}