#include <iostream>
#include <string>
#include <vector>

using namespace std;

class B64 {
    typedef struct  {
        //unsigned x :6;
        unsigned a : 6;
        unsigned b : 6;
        unsigned c : 6;
        unsigned d : 6;
    } triplet;
public:
    //template<typename t>
    string encodeKaputt(const string& in) const {
        auto tl = static_cast<decltype(in.size())>(in.size() * (4.0/3.0)) + 1;
        string tmp;
        cout << in.size() << " " << tl << endl;
        tmp.reserve(tl);
        const unsigned char *p = reinterpret_cast<const unsigned char*>(in.data());
        auto il = in.size();
        cout << sizeof(*p) << endl;
        for (;il >= 3; il-=3, p += 3) {
            cout << alpha_[reinterpret_cast<const triplet*>(p)->a];
            cout << alpha_[reinterpret_cast<const triplet*>(p)->b];
            cout << alpha_[reinterpret_cast<const triplet*>(p)->c];
            cout << alpha_[reinterpret_cast<const triplet*>(p)->d];
        }
        if (il >= 1) 
            cout << alpha_[reinterpret_cast<const triplet*>(p)->d];
        if (il >= 1)
            cout << alpha_[reinterpret_cast<const triplet*>(p)->c];
        if (il >= 2)
            cout << alpha_[reinterpret_cast<const triplet*>(p)->b];
        cout << endl;
        return tmp;
    }
    static string encode(const string& in) {
        auto tl = static_cast<decltype(in.size())>(in.size() * (4.0/3.0)) + 1;
        string tmp;
        tmp.reserve(tl);
        const unsigned char *p = reinterpret_cast<const unsigned char*>(in.data());
        auto il = in.size();
        for (;il >= 3; il-=3, ++p) {
			tmp.push_back(alpha_[*p >> 2]);
			tmp.push_back(alpha_[((*p & 0x03) << 4) + ((*(p+1) & 0xF0)>>4)]); ++p;
			tmp.push_back(alpha_[((*p & 0x0F) << 2) + ((*(p+1) & 0xC0)>>6)]); ++p;
			tmp.push_back(alpha_[*p & 0x3F]);
        }
        if (il >= 1) 
			tmp.push_back(alpha_[*p >> 2]);
        if (il >= 1) {
			tmp.push_back(alpha_[((*p & 0x03) << 4) + ((*(p+1) & 0xF0)>>4)]); ++p;
		}
        if (il >= 2) {
			tmp.push_back(alpha_[((*p & 0x0F) << 2) + ((*(p+1) & 0xC0)>>6)]); ++p;
		}
		while(tmp.size() % 4 != 0) {
			tmp.push_back(fill_);
		}
        return tmp;
    }
	explicit B64(ostream& o) : os_{o} {
		
	}
	B64(const B64&) = delete;
	B64(B64&& o) : bufferlen_{o.bufferlen_}, os_{o.os_} {
		buffer_[0] = o.buffer_[0];
		buffer_[1] = o.buffer_[1];
		buffer_[2] = o.buffer_[2];
	}
	friend ostream& operator<<(ostream& )
	void flush();
private:
    constexpr static const char* const alpha_ {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	constexpr static const char fill_ {'='};
	unsigned char buffer_[3] {'\0'};
	unsigned char bufferlen_ {0};
	ostream& os_;
};

int main(int argn, const char* const argv[])
{
	vector<string> param(argv, argv+argn);
   //B64 b;
	
	for(const auto& s : param) {
		cout << s << ": " << B64::encode(s) << endl;
	}
   return 0;
}

