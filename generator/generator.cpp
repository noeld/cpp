#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <random>
#include <memory>
#include <iterator>
#include <array>
#include <chrono>
#include <atomic>
extern "C" {
#include <unistd.h>
}
using namespace std;

class BadFilenameException : public std::exception
{
public:
	explicit BadFilenameException (const string& filename) : filename_(filename) {}	// type conversion
	BadFilenameException();									// default ctor
	BadFilenameException(const BadFilenameException& rhs) : filename_(rhs.filename_) {};							// copy ctor
	BadFilenameException(BadFilenameException&& rhs) : filename_(std::move(rhs.filename_)) {};								// move ctor
	BadFilenameException& operator=(const BadFilenameException& rhs) { filename_ = rhs.filename_; return *this; };				// copy assignment
	BadFilenameException& operator=(BadFilenameException&& rhs) { filename_ = std::move(rhs.filename_); return *this; };					// move assignment
	virtual ~BadFilenameException () =default;
	virtual const char* what() const noexcept override {
		return filename_.data();
	}

private:
	std::string filename_ {"BadFilename"};
};

class FileList
{
public:
	explicit FileList (const std::string& filename)	// type conversion
	: filename_(filename) {
		std::ifstream in(filename_);
		if (!in.is_open() || !in.good()) {
			throw BadFilenameException(filename_);
		}
		std::string buffer;
		while(in.good()) {
			getline(in, buffer);
			entries_.emplace_back(buffer);
		}
	}
	FileList() =delete;									// default ctor
	FileList(const FileList&) =delete;							// copy ctor
//	FileList(FileList&& rhs) : filename_(std::move(rhs.filename_), entries_(rhs.entries_)) { };								// move ctor
	FileList& operator=(const FileList&) =delete;				// copy assignment
	FileList& operator=(FileList&&) =delete;					// move assignment
	virtual ~FileList () {}
	const std::string& operator[](int i) const {
		if (0 > i || i >= entries_.size()) {
			throw std::out_of_range("Out of Range: " + std::to_string(i));
		}
		//std::cout << i << "- ";
		return entries_[i];
	}
	size_t size() const noexcept { return entries_.size(); }
	const decltype(std::vector<std::string>{}.begin()) begin() { return entries_.begin(); }
	const decltype(std::vector<std::string>{}.end()) end() { return entries_.end(); }

private:
	std::string filename_;
	std::vector<std::string> entries_;
	
	//std::default_random_engine generator {static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())};
};

template<typename T>
class RandomElementIterator 
{
public:
	explicit RandomElementIterator (const T& fl)	// type conversion
		: fl_(&fl)
		, dre_(std::chrono::system_clock::now().time_since_epoch().count()) 
		, dist_(0, fl.size()-1)
		{}
	RandomElementIterator()=delete;									// default ctor
	RandomElementIterator(const RandomElementIterator& rhs) : fl_{rhs.fl_}, index_{rhs.index_}, dist_(rhs.dist_)		// copy ctor
	{}
	RandomElementIterator(RandomElementIterator&& rhs) : fl_{rhs.fl_}, index_{rhs.index_}, dist_(rhs.dist_)				// move ctor
	{
		rhs.fl_ = nullptr;
		rhs.index_ = 0;
	}
	RandomElementIterator& operator=(const RandomElementIterator& rhs) 				// copy assignment
	{
		if (&rhs != this) {
			fl_ = rhs.fl_;
			index_ = rhs.index_;
			dist_ = rhs.dist_;
		}
		return *this;
	}
	RandomElementIterator& operator=(RandomElementIterator&& rhs)					// move assignment
	{
		if (&rhs != this) {
			fl_ = rhs.fl_;
			index_ = rhs.index_;
			dist_ = std::move(rhs.dist_);
			rhs.fl_ = nullptr;
			rhs.index_ = 0;
		}
		return *this;
	}
	virtual ~RandomElementIterator () = default;
	
	size_t operator*() const {
		return index_;
	}
	
	size_t operator++() {
		index_ = dist_(dre_);
		//std::cerr << __func__ << ": " << index_ << std::endl;
		return index_;
	}
	
	size_t operator++(int) {
		auto tmp = index_;
		index_ = dist_(dre_);
		//std::cerr << __func__ << ": " << tmp << std::endl;
		return tmp;
	}
private:
	const T* fl_;
	size_t	index_{0};
	std::default_random_engine dre_;
	std::uniform_int_distribution<size_t> dist_;
	
};




class StoppWatch
{
public:
	StoppWatch() =default;									// default ctor
	StoppWatch(const StoppWatch&) =default;							// copy ctor
	StoppWatch& operator=(const StoppWatch&) =default;				// copy assignment
	virtual ~StoppWatch () {
		std::cerr << "Dauer: " << this->duration() << " Sekunden" << std::endl;
	}
	double duration() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_).count()/1000.0;
	}
private:
	std::chrono::high_resolution_clock::time_point start_ { std::chrono::high_resolution_clock::now()};
} stoppwatch;

int main (int argc, char const *argv[])
{
	const std::string wvornamen {"data/wvornamen.txt"};
	const std::string mvornamen {"data/mvornamen.txt"};
	const std::string nachnamen {"data/nachnamen.txt"};
	const std::string plz {"data/OpenGeoDB_bundesland_plz_ort_de.csv"};
	int ret = 0;
	int max = 1;
	if (argc > 1) {
		int n = stoi(argv[1]);
		if (n > 0) max = n;
	}
	try {

		std::array<char, 2> ge {{'M', 'W'}};
		FileList flwv(wvornamen); std::cerr << flwv.size() << " weibliche Vornamen\n";
		FileList flmv(mvornamen); std::cerr << flmv.size() << " männliche Vornanen\n";
		FileList flnn(nachnamen); std::cerr << flnn.size() << " Nachnamen\n";
		FileList flplz(plz); std::cerr << flplz.size() << " Postleitzahlen\n";
		std::mutex mtx;
		std::atomic<uint32_t> gugl{0}, gngc{0};
		std::atomic<uint32_t> line{0};
		std::atomic<uint64_t> rthreads {0x0000000000000001};
		constexpr const int lintervall {5000};
		
		auto ggg = [&ge, &flwv, &flmv, &flnn, &flplz, &mtx, &line, &gugl, &gngc, &rthreads] (uint32_t idfrom, uint32_t idto, short nr) {
			rthreads.fetch_or(1 << nr);
			std::ostringstream tmpout;
			int seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator(seed);
			std::uniform_int_distribution<int> gedist(0, 1);
			//auto rnge = std::bind(gedist, generator);
		
			RandomElementIterator<decltype(ge)> geit(ge);
		
			std::geometric_distribution<int> geodist(1.0/static_cast<double>(250));
			uint32_t ugl {0}, ngc {0};
			auto rngeo = [&geodist, &generator, &ugl, &ngc](int max){ ++ngc; int i; while(max <= (i = geodist(generator))) { ++ugl; }
				; return i; };
			std::uniform_int_distribution<int> plzdist(0, flplz.size()-1);
			auto rnplz = std::bind(plzdist, generator);
			const char* trenn = ";";
			auto lastid = idfrom;
			for (auto i = idfrom; i < idto; ++i) {
				//int g = rnge();
				int g = ++geit;
				char geschl = ge[g];
				tmpout << i << trenn << geschl << trenn; 
				if ('M' == geschl) { tmpout << flmv[rngeo(flmv.size()-1)]; } else { tmpout << flwv[rngeo(flwv.size()-1)]; } 
				tmpout << trenn << flnn[rngeo(flnn.size()-1)] << trenn << flplz[rnplz()] << std::endl;
				if (i % lintervall == 0) {
					{
						std::lock_guard<std::mutex> lock(mtx);
						std::cout << tmpout.str();
					}
					tmpout.str("");
					line+=i-lastid;
					lastid=i;
				}
			}
			{
				std::lock_guard<std::mutex> lock(mtx);
				std::cout << tmpout.str();
			}
			line+=idto-lastid;
			gugl+=ugl;
			gngc+=ngc;
			rthreads.fetch_xor(1 << nr);
		};
		
		{
			auto mmax = std::min(std::thread::hardware_concurrency(), 64u);
			uint32_t partition = max / mmax;
			uint32_t s = 0;
			std::vector<std::thread> threads;
			for(uint32_t i = 0; i < mmax-1; ++i, s+=partition) {
				std::cerr << "Thread " << i << " von " << s << " bis " << s + partition - 1 << endl;
				threads.emplace_back(ggg, s, s+partition, i);
			}
			std::cerr << "Thread " << mmax-1 << " von " << s << " bis " << max - 1 << endl;
			threads.emplace_back(ggg, s, max, mmax-1);
			std::cerr.precision(3);
			while(rthreads != 0) {
				usleep(125000);
				uint32_t tmp = line;
				std::cerr << "\r" << tmp << " (" << rthreads << ") " << (1.0*line)/max*100.0<<"% "; std::cerr.flush();
			}
			std::cerr << std::endl;
			for(auto& t : threads) {
				t.join();
			}
		}
		
		std::cerr << "\nGenerator calls: " << gngc << ", unnessesary generator loop: " << gugl << std::endl;
		std::cerr << static_cast<int>(max/stoppwatch.duration()) << "/sec" << std::endl;
		//const std::string 
		
		
	} catch(std::exception& e) {
		std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Unbekannte Ausnahme\n";
	}
	return ret;
}