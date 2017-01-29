//# /Users/arnold/Documents/prg/clang/bin/clang++ -U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -g -L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib  -o base64 base64.cpp -lboost_system -lboost_filesystem -lboost_iostreams  

#include <iostream>
#include <string>
#include <vector>

using namespace std;

string alpha{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

static constexpr const array<uint32_t, 256> rev_ {
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x03eu, 0x100u, 0x100u, 0x100u, 0x03fu,
0x034u, 0x035u, 0x036u, 0x037u, 0x038u, 0x039u, 0x03au, 0x03bu,   0x03cu, 0x03du, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x000u, 0x001u, 0x002u, 0x003u, 0x004u, 0x005u, 0x006u,   0x007u, 0x008u, 0x009u, 0x00au, 0x00bu, 0x00cu, 0x00du, 0x00eu,
0x00fu, 0x010u, 0x011u, 0x012u, 0x013u, 0x014u, 0x015u, 0x016u,   0x017u, 0x018u, 0x019u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x01au, 0x01bu, 0x01cu, 0x01du, 0x01eu, 0x01fu, 0x020u,   0x021u, 0x022u, 0x023u, 0x024u, 0x025u, 0x026u, 0x027u, 0x028u,
0x029u, 0x02au, 0x02bu, 0x02cu, 0x02du, 0x02eu, 0x02fu, 0x030u,   0x031u, 0x032u, 0x033u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,

0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,
0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u,   0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u, 0x100u
};
static void ReverseAlpha() {
    vector<unsigned> r(0x100, 0x100u);
    size_t i = 0;
    for(const char& c : alpha_) {
        r[static_cast<unsigned>(c)] = i;
        ++i;
    }
    for(i = 0; i < r.size(); ++i) {
        printf("0x%03xu, ", r[i]);
        switch(i % 16) {
            case 7: printf("  ");
                break;
            case 15: printf("\n");
                break;
        }
    }
}

string base64_encode(const string& in) 	{
	string result;
	size_t len = 4 * (in.size() / 3 + ( in.size() % 3 > 0 ? 1 : 0));
	result.reserve(len);
	auto b = in.begin();
	while(b != in.end()) {
		unsigned q = static_cast<unsigned>(*b) << 16;
		++b;
		if (b != in.end()) {
			q += static_cast<unsigned>(*b) << 8;
			++b;
			if (b != in.end()) {
				q += static_cast<unsigned>(*b);
				++b;
			}
		}
		result += alpha[ (q >> 18) & 0x3f ];
		result += alpha[ (q >> 12) & 0x3f ];
		result += alpha[ (q >>  6) & 0x3f ];
		result += alpha[  q        & 0x3f ];
	}
	auto m = in.size() % 3;
	if (m > 0)
		result[result.size() - 1] = '=';
	if (m == 1)
		result[result.size() - 2] = '=';
	return result;
}

string base64_decode(const string& in) {
	vector<unsigned> ralpha(0x100, 0x100);
	size_t i = 0;
	for(auto c : alpha) {
		ralpha[static_cast<unsigned>(c)] = i++; 
	}
	auto b = in.begin();
	if((in.end() - b) % 4 != 0) 
		throw "Length of input to base64_decode must be a multiple of 4!";
	string result;
	size_t len = in.size() / 4 * 3;
	result.reserve(len);
	while(b != in.end()) {
		unsigned n = 4;
		unsigned q1 = ralpha[static_cast<unsigned>(*b)]; //cout << 1 << *b << " - " << static_cast<unsigned>(*b) << " - " << q1 << endl;
		if (q1 == 0x100 || *b == '=')
			throw "Illegal character";
		++b;
		unsigned q2 = ralpha[static_cast<unsigned>(*b)]; //cout << 2 << *b << " - " << static_cast<unsigned>(*b) << " - " << q2 << endl;
		if (q2 == 0x100 || *b == '=')
			throw "Illegal character";
		++b;
		unsigned q3 = ralpha[static_cast<unsigned>(*b)]; //cout << 3 << *b << " - " << static_cast<unsigned>(*b) << " - " << q3 << endl;
		if (*b == '=') {
			q3 = 0;
			--n;
		} else if (q3 == 0x100)
			throw "Illegal character";
		++b;
		unsigned q4 = ralpha[static_cast<unsigned>(*b)]; //cout << 4 << *b << " - " << static_cast<unsigned>(*b) << " - " << q4 << endl;
		if (*b == '=') {
			q4 = 0;
			--n;
		} else if (q4 == 0x100)
			throw "Illegal character";
		++b;
		unsigned ii = (q1 << 18) + (q2 << 12) + (q3 << 6) + q4;
		result += (ii >> 16) & 0xff;
		if (n > 2)
			result += (ii >>  8) & 0xff;
		if (n > 3)
			result += (ii      ) & 0xff;
	}
	return result;
}

int main(int argc, char const *argv[])
{
	try {
		string tt{"Das ist ein Test."};
		string test = tt;
		if (argc > 1) {
			test = argv[1];
		}
		string result = base64_encode(test);
		
		cout << test << endl;
		cout << result << endl;
		cout << base64_decode(result) << endl;
	} catch (const char* e) {
		cerr << e << endl;
	}
	return 0;
}