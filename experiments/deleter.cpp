// Use as unique_ptr and shared_ptr as a ressource handle
// cmake -DCMAKE_CXX_COMPILER=g++-7 -DCMAKE_C_COMPILER=gcc-7 -G 'Sublime Text 2 - Unix Makefiles' ..
// cmake --build . -- -j 2

#include <memory>
#include <cstdio>

int main(int argc, char const *argv[])
{
	int ret = 0; 
	const char* filename = "../deleter.cpp";
	// build a unique_ptr which automatically calls fclose when it goes out of
	// scope.
	std::unique_ptr<FILE, decltype(&std::fclose)> pfile(std::fopen(filename, "r"), std::fclose);
	if (pfile) {
		using namespace std;
		printf("Opened file %s\n", filename);
	} else {
		extern int errno;
		printf("Could not open file %s; error %d\n", filename, errno);
	}

	// build an unique_ptr which calls a lambda when it goes out of scope.
	auto del = [=](FILE* f) { std::fclose(f); printf("Closed file\n"); };
	std::unique_ptr<FILE, decltype(del)> pf2(std::fopen(filename, "r"), del);
	
	// build shared_ptr the same way as the unique_ptr-s, which call fclose or
	// a lambda expression when the pointed to shall be deleted.
	std::shared_ptr<FILE> sf1(std::fopen(filename, "r"), &std::fclose);
	std::shared_ptr<FILE> sf2(std::fopen(filename, "r"), del);

	return ret;
}