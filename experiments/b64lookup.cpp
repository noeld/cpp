#include<boost/format.hpp>
#include<iostream>
#include<array>
#include <string>
#include <cstdint>
// constexpr const std::array<char, 64> alpha {
//     'A', 'B'
// }

struct Put {
    size_t i = 0;
    size_t elt_per_line = 16;
    size_t elt_per_col  = 8;
    std::string elem_separator {", "};
    std::string mid_space      {"  "};
    std::string prefix         {"    "};
    template<typename T>
    void operator()(const T& c) { 
        using namespace std;
        if (i > 0)
            cout << elem_separator;
        auto r = i % elt_per_line;
        if (r == 0)
            cout << endl << prefix;
        else if ((r % elt_per_col) == 0)
            cout << mid_space;
        cout << c;
        ++i;
    };
};

void generate_alpha(std::array<int, 256>& ralpha) {
    using namespace std;
    Put put;
    size_t val = 0;
    boost::format fmt{"'%c'"};
    auto process_range = [&](char from, char to) {
        for(char c = from; c<=to; ++c) {
            put(fmt % c);
            ralpha[static_cast<size_t>(c)] = val++;
        }
    };
    process_range('A', 'Z');
    process_range('a', 'z');
    process_range('0', '9');
    process_range('+', '+');
    process_range('/', '/');
}
constexpr const std::array<char, 64> alpha= {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',   'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
    'w', 'x', 'y', 'z', '0', '1', '2', '3',   '4', '5', '6', '7', '8', '9', '+', '/'
};
constexpr const std::array<uint8_t, 256> ralpha = {
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0x3e,   0xff, 0xff, 0xff, 0x3f, 
    0x34, 0x35, 0x36, 0x37,   0x38, 0x39, 0x3a, 0x3b,   0x3c, 0x3d, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0x00, 0x01, 0x02,   0x03, 0x04, 0x05, 0x06,   0x07, 0x08, 0x09, 0x0a,   0x0b, 0x0c, 0x0d, 0x0e, 
    0x0f, 0x10, 0x11, 0x12,   0x13, 0x14, 0x15, 0x16,   0x17, 0x18, 0x19, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0x1a, 0x1b, 0x1c,   0x1d, 0x1e, 0x1f, 0x20,   0x21, 0x22, 0x23, 0x24,   0x25, 0x26, 0x27, 0x28, 
    0x29, 0x2a, 0x2b, 0x2c,   0x2d, 0x2e, 0x2f, 0x30,   0x31, 0x32, 0x33, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff,   0xff, 0xff, 0xff, 0xff
};

void encode(const std::string& input, std::string& output) {
    size_t i = 0;
    uint8_t buffer[3];
    char obuffer[4];
    for(const uint8_t* it = reinterpret_cast<const uint8_t*>(input.data()), *end = it + input.size();
        it < end;
        ++it) {
    // for(const auto& e : input) {
        buffer[i] = *it;
        ++i;
        if (i % 3 == 0) {
            obuffer[0] = alpha[  (buffer[0] >> 2) ];
            obuffer[1] = alpha[ ((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)  ];
            obuffer[2] = alpha[ ((buffer[1]       ) << 4) + (buffer[2] >> 6)  ];
            obuffer[3] = alpha[  (buffer[2] & 0x3f) ];
            output.append(&obuffer[0], 4);
            i = 0;
        }
    }
}

int main(int argc, char const *argv[])
{
    using namespace std;
    std::ios_base::sync_with_stdio(false);
    std::array<int, 256> ralpha;
    std::fill(ralpha.begin(), ralpha.end(), 0xff);
    cout << "constexpr const std::array<char, 64> alpha = {";
    generate_alpha(ralpha);
    cout << endl << "};" << endl;
    cout << "constexpr const std::array<uint8_t, 256> ralpha = {";
    boost::format fmt{"0x%02x"};
    Put put;
    put.elt_per_col  = 4;
    for(auto& e : ralpha) {
        put(fmt % e);
    }
    cout << endl << "};" << endl; 
    std::string input{"Hallo!"};
    if (argc > 1)
        input = argv[1];
    cout << "Encoding " << input << "(" << input.size() << ") chars." << endl;
    std::string output;
    encode(input, output);
    cout << input << " -> " << output << endl;
    return 0;
}

/*

#include <iostream>
#include <algorithm>
#include <numeric>
#include <array>
using namespace std;

template<typename T>
void build_alpha(T& buffer) {
    auto begin = buffer.begin();
    auto fill = [&begin](char start, char last) {
        auto end = begin + (last - start) + 1;
        std::iota(begin, end, start);
        begin = end;
    };
    fill('A', 'Z');
    fill('a', 'z');
    fill('0', '9');
    *(begin++) = '+';
    *(begin++) = '/';
}

template<typename T, size_t N>
ostream& print_array(ostream& o, const array<T, N>& t) {
    for(size_t i = 0; i < t.size(); ++i) {
        cout << '\'' << t[i] << '\'';
        auto m = i % 16;
        const char* sep = ", ";
        switch(m) {
            case 3: //fallthrough
            case 11: sep = ",  "; break;
            case 7: sep = ",   "; break;
            case 15: sep = ",\n"; break;
        }
        cout << sep;
    }
}

constexpr const array<const char, 64> al {
    'A', 'B', 'C', 'D',  'E', 'F', 'G', 'H',   'I', 'J', 'K', 'L',  'M', 'N', 'O', 'P',                                                               
    'Q', 'R', 'S', 'T',  'U', 'V', 'W', 'X',   'Y', 'Z', 'a', 'b',  'c', 'd', 'e', 'f',                                                               
    'g', 'h', 'i', 'j',  'k', 'l', 'm', 'n',   'o', 'p', 'q', 'r',  's', 't', 'u', 'v',                                                               
    'w', 'x', 'y', 'z',  '0', '1', '2', '3',   '4', '5', '6', '7',  '8', '9', '+', '/'
};

int main()
{
   array<char, 64> buffer;
   build_alpha(buffer);
   print_array(cout, buffer);
   print_array(cout, al);
   return 0;
}

*/