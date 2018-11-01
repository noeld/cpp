#include <iostream>
#include <climits>
#include <bitset>

template<typename T>
struct PrintBits final {
    PrintBits(const T& t) : t_{t} {}
    friend std::ostream& operator<<(std::ostream& o, const PrintBits& b) {
        decltype(auto) it = reinterpret_cast<char const *>(&b.t_);
        const auto end = it + sizeof(T);
        o << "( " << sizeof(T) << " " << typeid(T).name() << ": ";
        if (it != end) {
          o << std::bitset<CHAR_BIT>(*it);
          ++it;
        }
        while(it != end) {
            o << ' ' << std::bitset<CHAR_BIT>(*it) ;
            ++it;
        }
        o << " )";
        return o;
    }
    const T& t_;
};

int main(int argc, char const *argv[])
{
    using namespace std;
    union {
        double d;
        uint64_t ui;
    } x = { 3.1415};
    cout << PrintBits<double>(x.d) << endl;
    cout << PrintBits<int>(63) << endl;
    return 0;
}