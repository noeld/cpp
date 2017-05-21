#include <iostream>
#include "Base64.h"

template<typename T>
void HexPrintRange(const T& r) {
    size_t i = 0;
    for(auto&& c : r) {
        const char* d;
        switch(++i % 16) {
            case 8:
                d = ",  ";
                break;
            case 0:
                d = ",\n";
                break;
            default:
                d = ", ";
        }
        printf("0x%02x%s", c, d);
    }
    printf("\n");
}

int main() {
    //HexPrintRange(Base64::alpha_);
    //Base64::BuildReverseAlpha(Base64::alpha_, Base64::ralpha_);
    //std::cout << "---------------" << std::endl;
    //HexPrintRange(Base64::ralpha_);
    Base64::conversion_t t, t2;
    try {
        t.cu = Base64::EncodeByteTriplet((unsigned char *) "123");
        printf("%c%c%c%c\n", t.cc[0], t.cc[1], t.cc[2], t.cc[3]);
        t2.cu = Base64::DecodeCharQuadruplet(t.cc);
        printf("%c%c%c\n", t2.cc[0], t2.cc[1], t2.cc[2]);
        std::string s;
        std::cin >> std::noskipws;
        while(1) {
            getline(std::cin, s);
            if (s == "-")
                break;
            auto r = Base64::Encode(s);
            std::cout << r << std::endl;
            std::cout << Base64::Decode(r) << std::endl;
        }
    } catch(const char* e) {
        std::cerr << e << std::endl;
    }
    return 0;
}