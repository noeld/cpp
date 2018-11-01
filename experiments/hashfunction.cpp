#include <iostream>
#include <string>
#include <utility>
#include <unordered_set>

struct MyTest {

    friend class std::hash<MyTest>;

    friend bool operator==(const MyTest& a, const  MyTest& b) {
        return (a.a_ == b.a_) && (a.str_ == b.str_);
    }

    friend bool operator<(const MyTest& a, const  MyTest& b) {
        return (a.a_ < b.a_) && (a.str_ < b.str_);
    }

    int         a_   { 1       };
    std::string str_ { "Hallo" };
};

namespace std {
    template<>
    struct hash<MyTest> {
        size_t operator()(const MyTest& t) const {
            return std::hash<int>().operator()(t.a_) ^ std::hash<std::string>().operator()(t.str_);
        }
    };
}


int main(int argc, char const *argv[])
{
    std::unordered_set<MyTest> s;
    s.insert({2, "Bla"});
    s.insert({2, "Bla"});
    s.insert({1, "Huhu"});
    s.insert({3, "Blubb"});
    s.insert({3, "Blubb"});
    for(const auto& e : s) {
        std::cout << e.a_ << "-" << e.str_ << std::endl;
    }
    return 0;
}