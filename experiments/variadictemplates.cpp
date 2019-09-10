// try out variadic templates and template parameter packs etc

#include<iostream>
#include<utility>

using namespace std;

struct my_type {};
ostream& operator<<(ostream& o, my_type const & m) 
{
    return o << "my_type!!!";
}

// helper function to terminate template expansion
inline
void print_anything() 
{}

// print anything
template<typename T, typename ... Ts>
inline
void print_anything(T&& t, Ts&&... ts)
{
    cout << typeid(T).name() << ": " << t;
    // statically compile in a separator if there are still more ts
    if constexpr (sizeof...(ts) > 0)
        cout << ", ";
    // print_anything(forward<T>(t));
    print_anything(forward<Ts>(ts)...);
}

int main(int argc, char const *argv[])
{
    int ret = 0;
    cout << "print_anything(); ";
    print_anything();
    cout << endl;

    cout << "print_anything(1); ";
    print_anything(1);
    cout << endl;

    cout << "print_anything(\"Hallo\"); ";
    print_anything("Hallo");
    cout << endl;

    cout << "print_anything(\"Hallo\", 1, 3.1415926535, my_type()); ";
    print_anything("Hallo", 1, 3.1415926535, my_type()); 
    cout << endl;

    return ret;
}