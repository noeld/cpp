// interpret raw data with a hierarchy of helper classes

#include <iostream>

enum raw_block_type : uint8_t { LEAF = 1, NODE = 2, FILEHEADER = 3 };

const uint32_t raw_magic = 0xabcdefab;

struct alignas(uint32_t) raw_block_header {
   const uint32_t magic               { raw_magic };
         uint32_t block_number        { 1 };
         uint32_t parent_block_number { 0 };
   raw_block_type type                { LEAF };
};

const size_t raw_block_size = 4096;

struct alignas(uint32_t) raw_block {
    raw_block_header header;
    char             payload[ raw_block_size - sizeof(raw_block_header)];
};

struct alignas(uint32_t) block_fileheader {
    char     filename[64];
    uint32_t root_block_number { 1 };
};

#define PS(x) std::cout << "sizeof(" #x ") = " << sizeof(x) << std::endl;

int main(int argc, char const *argv[])
{
    using namespace std;
    PS(raw_block);
    return 0;
}
