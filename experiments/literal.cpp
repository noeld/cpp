#include <iostream>
#include <string>
#include <boost/variant.hpp>
constexpr int operator ""_neg(unsigned long long i) { return -i; }

 int operator ""_neg(const char* c) {
    return -std::stoi(c);
}

 int operator ""_neg(const char* c, size_t n) {
    return -std::stoi(c);
}


int main(int argc, char const *argv[])
{
    using namespace std;

    cout << 12345_neg << endl;
    cout << 123456678_neg << endl;
    cout << "1234566789"_neg << endl;
    cout << sizeof(string) << endl;

    using v_t = boost::variant<string, int>;
    cout << sizeof(v_t) << endl;
    // cout << sizeof(variant<string>) << endl;
    // cout << sizeof(variant<string, int>) << endl;
    return 0;
}