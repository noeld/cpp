// https://de.wikipedia.org/wiki/Internationale_Bankkontonummer#Zusammensetzung

#include<iostream>
#include<string>
#include<regex>
#include<boost/multiprecision/gmp.hpp>

void usage() {
    std::cout << "iban <iban>" << std::endl 
              << std::endl
              << "check iban" << std::endl;
}

std::string convert_lc(const std::string& lc) {
    static auto conv_digit = [=](char c) -> std::string {
        auto tmp = c - 'A' + 10;
        return std::to_string(tmp);
    };
    return conv_digit(lc[0]) + conv_digit(lc[1]);
}

int check_iban(const std::string iban) {
    int ret = 0;
    std::cout << "=== Checking " << iban << "===" << std::endl;
    const std::regex re ("[A-Z][A-Z]\\d{20}", std::regex::icase);
    if (! std::regex_match(iban, re)) {
        std::cout << "IBAN ist falsch formatiert" << std::endl;
        ret = 1;
    }
    const std::regex ree("^(..)(..)(.+)$");
    std::smatch m;
    if (std::regex_match(iban, m, ree)) {
        auto lc = m[1].str();
        auto pz = m[2].str();
        auto rest = m[3].str();
        std::cout << "LC: " << lc << std::endl;
        std::cout << "PZ: " << pz << std::endl;
        std::cout << "Rest: " << rest << std::endl;
        auto lcdigits = convert_lc(lc);
        std::cout << "LC digits: " << lcdigits << std::endl;
        using namespace std::string_literals;
        boost::multiprecision::mpz_int i{rest + lcdigits + "00"s};
        boost::multiprecision::mpz_int calc_pz = 98 - (i % 97); 
        auto calc_pz_str = calc_pz.convert_to<std::string>();
        std::cout << "98 - (" << i << " % 97) = " << calc_pz_str << std::endl;
        auto res = pz == calc_pz_str;
        std::cout << "Die Prüfziffern stimmen" << ( res ? "." : " nicht!") << std::endl;
        if (!res)
            ret = 1;
    }
    return ret;
}

// ./iban DE5837040044028465250 DE58370400440284652500
int main(int argn, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    if (argn == 1) {
        usage();
        exit(1);
    }
    int ret = 0;
    for (int i = 1; i < argn; ++i) {
        ret = std::max(ret, check_iban(argv[i]));
    }
    return ret;
}
