// ndimarray.cpp
// a generic n-dimensional array with proper access operators

// TODO
// [ ] generic value_type instead of int

#include<iostream>
#include<utility>

using namespace std;

template<size_t ... NS>
struct F {
    static constexpr size_t DIM { sizeof...(NS) };
    static constexpr size_t size();
    static constexpr size_t SIZE { size() };
    static constexpr int D[DIM] { NS... };
    using array_type = int[SIZE];
    array_type e_;
    template<typename ... Ss>
    static constexpr size_t index(Ss ... ss);
    array_type& operator()() & { return e_; }
};

// iterate over a non-type parameter pack
template<size_t ... NS>
constexpr
size_t F<NS...>::size() {
    size_t ret = 1;
    for(auto&& e : { NS...})
        ret *= e;
    return ret; 
}

// see https://stackoverflow.com/questions/23628876/add-all-parameters-with-parameter-pack-expansion#23630557
// template<size_t ... NS>
// template<typename ... Ss>
// constexpr 
// size_t F<NS...>::index(Ss ... ss) {
//     return (1 * ss)...;
// }

int main(int argc, char const *argv[])
{
    int ret = 0;
    F<2, 3, 4, 5> f;
    cout << f.size() << endl;
    for(auto&& e : decltype(f)::D)
        cout << e << "," << endl;
    int d[] = { 3, 4, 5};
    auto index = [d](int x, int y, int z) {
        return x + y * d[0] + z * d[0] * d[1];
    };
    auto index_from_arr = [d](int(&v)[3]) {
        auto di = std::begin(d);
        int f   = 1;
        int res = 0;
        for(auto e : v) {
            res += e * f;
            f   *= *di;
            ++di;
        }
        return res;
    };

    int v[3] = {1, 2, 3};
    auto r = index_from_arr(v);
    cout << " :: " << r << endl;
    return ret;
}