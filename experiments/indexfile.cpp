// Index a file by line numbers
#include <iostream>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <string>
// #include <string_view>
#include <iterator>
#include <algorithm>
#include <vector>

class FileIndex
{
    static constexpr char const * std_file_name = "/Users/arnold/cpp/experiments/zhang.cpp.bak";
public:
    FileIndex(const std::string& file_name = std_file_name)
    : mmf_(boost::interprocess::open_only, file_name.data())
    {
    }
    ~FileIndex() =default;
private:
    char const * begin() const { return reinterpret_cast<char const *>(mmf_.get_address()); }
    char const * end() const { return begin() + mmf_.get_size(); }
    void build_index() {
        for (auto i = this->begin(), ii = i; i != this->end(); ++i)
        {
            if (*i == '\n') {
                pos_.push_back(i - ii);
            }
        }
    }
public:
    const std::vector<size_t>& get() const { return pos_; };
private:
    boost::interprocess::managed_mapped_file mmf_;
    std::vector<size_t> pos_;
};

int main(int argc, char const *argv[])
{
    using namespace std; 
    try {
        FileIndex fi;
        for(auto a : fi.get()) {
            cout << a << endl;
        }
    } catch(const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }    
    return 0;
}