//g++  5.4.0

#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <cstdlib>

int roman_to_int(char c) {
    switch(c) {
        case 'I' : return 1;
        case 'V' : return 5;
        case 'X' : return 10;
        case 'L' : return 50;
        case 'C' : return 100;
        case 'D' : return 500;
        case 'M' : return 1000;
        default: throw "Illegal roman digit";
    }
}

int roman_to_int(const std::string& s) {
    int previous = 0; //roman_to_int('M');
    int res = 0;
    for(auto e = s.rbegin(); e != s.rend(); ++e) {
        auto d = roman_to_int(*e);
        res += ( d < previous ) ? -d : d;
        previous = d;
        //std::cout << *e << " : " << res << std::endl;
    }
    return res;
}

struct RC { int i; wchar_t c; int i5; wchar_t c5; };
const std::array< RC, 4> rc {{
    { 1, L'I', 5, L'V'}
    , {10, L'X', 50, L'L'}
    , {100, L'C', 500, L'D'}
    , {1000, L'M', 5000, L'ↁ'}
}};

std::wstring int_to_roman(unsigned i) {
    std::wstring s;
    for(auto d = rc.rbegin(); d != rc.rend(); ++d) {
        // std::cerr << "i: " << i << " d->i " << d->i;
        auto remquot = std::div(i, d->i);
        // std::cerr << "remquot.quot" << remquot.quot << std::endl;
        switch (remquot.quot) {
            case 0: break;
            case 1:
            case 2:
            case 3: s.append(remquot.quot, d->c); break;
            case 4: s += d->c; s+= d->c5; break;
            case 5: 
            case 6:
            case 7:
            case 8: s += d->c5; s.append(remquot.quot - 5, d->c); break;
            case 9: if (d == rc.rbegin()) {
                        // std::cout << "d == rc.rbegin()" << std::endl;
                        s += d->c5; s.append(remquot.quot - 5, d->c);
                    } else {
                        s += d->c; 
                        s += (d-1)->c; 
                    }
                    break;
            default: throw "das hätte nich passieren dürfen";
        }
        i -= remquot.quot * d->i;
        // std::cerr << s << " " << i << std::endl;
    }
    return s;
}

int main(int argn, char* argv[])
{
    std::cout << roman_to_int('L') << std::endl;
    std::cout << roman_to_int("MMMCMLXXXVI") << std::endl;
    std::cout << roman_to_int("MMIM") << std::endl;
    int max = 1000;
    if (argn > 1)
        max = std::atoi(argv[1]);
    for(int i = 1; i < max; ++i) {
        std::wcout << i << ": " << int_to_roman(i);
        // auto u = roman_to_int(int_to_roman(i));
        // if (i == u)
        //     std::cout << " OK";
        // else std::cout << " Fehler";
        std::wcout << std::endl;
    }
}