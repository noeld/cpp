#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <algorithm>
#include <numeric>
#include <boost/format.hpp>
extern "C" {
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <unistd.h>
}

char const * const file_name = "/Users/arnold/Downloads/CentOS-7-x86_64-DVD-1708.iso";  
//char const * const file_name = "/Users/arnold/Downloads/SmartSwitch_for_Mac.dmg";  

template<typename T, typename D>
class AutoHandle {
public:
    AutoHandle(const T& handle, D&& deleter, bool(&&success)(T)) 
    : handle_{handle}
    , deleter_{std::forward<D>(deleter)}
    , success_{std::forward<bool(&)(T)>(success)}
    {
        if (!success_(handle_)) {
            throw "Fehler";
        }
    }
    ~AutoHandle() noexcept {
        if (success_(handle_)) {
            deleter_(handle_);
        }
    }
    AutoHandle& operator=(const AutoHandle&) = delete;
    operator const T&() const noexcept { return handle_; }
    const T& get() const noexcept { return handle_; }
private:
    const T handle_;
    const D& deleter_;
    bool(&success_)(T);
};

bool check_fd(int i) { return i > -1; }
bool check_mm(void* p) { return p != MAP_FAILED; }

int main(int argc, char const *argv[])
{
    int ret = 0;
    char const* fname = file_name;
    if (argc > 1)
        fname = argv[1];
    std::ios_base::sync_with_stdio(false);
    try {
        AutoHandle<int, decltype(close)> ahF(open(fname, O_RDONLY), close, check_fd);
        struct stat st;
        fstat(ahF, &st);
        std::cout << "File size: " << st.st_size << std::endl;
        const auto fct_unmap = [st](void* p){munmap(p, st.st_size);};
        AutoHandle<void*, decltype(fct_unmap)> ahMM(
              mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE | MAP_FILE, ahF, 0)
            , std::forward<decltype(fct_unmap)>(fct_unmap)
            , check_mm);
        std::cout << "Ptr: " << ahMM.get() << std::endl;
        std::array<size_t, 0x100> hist;
        std::fill(hist.begin(), hist.end(), 0);
        std::for_each( static_cast<uint8_t const *>(ahMM.get())
                     , static_cast<uint8_t const *>(ahMM.get()) + st.st_size
                     , [&hist](const uint8_t& e) { ++hist[e]; } );
        float sum = static_cast<float>(std::accumulate(hist.begin(), hist.end(), 0));
        std::array<float, 0x100> histpcnt;
        std::transform(hist.begin(), hist.end(), histpcnt.begin()
            , [sum](const size_t& i){ return i / sum * 100.0f; }
            );
        boost::format fmt{"%02x: %7d (%04.1f%%)"};
        for(auto i = 0; i < hist.size(); ++i) {
            std::cout << fmt % i % hist[i] % histpcnt[i]
                      << ((i % 8 == 7) ? "\n" : "  ");
        }
    } catch(char const * const e) {
        std::cout << "Fehler: " << e << std::endl;
    }
    return ret;
}