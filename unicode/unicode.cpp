#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <array>
#include <bitset>
#include <memory>

using namespace std;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class UTF8 {
public:
	static constexpr const unsigned char PREFIX_FB1 = 0x00;
	static constexpr const unsigned char PREFIX_FB2 = 0xc0;
	static constexpr const unsigned char PREFIX_FB3 = 0xe0;
	static constexpr const unsigned char PREFIX_FB4 = 0xf0;
	static constexpr const unsigned char PREFIX_CB  = 0x80;
	static constexpr const unsigned char PMASK_FB1  = 0x80;
	static constexpr const unsigned char PMASK_FB2  = 0xe0;
	static constexpr const unsigned char PMASK_FB3  = 0xf0;
	static constexpr const unsigned char PMASK_FB4  = 0xf8;
	static constexpr const unsigned char PMASK_CB   = 0xc0;
	static constexpr const unsigned char MASK_FB1   = 0x7f;
	static constexpr const unsigned char MASK_FB2   = 0x1f;
	static constexpr const unsigned char MASK_FB3   = 0x0f;
	static constexpr const unsigned char MASK_FB4   = 0x07;
	static constexpr const unsigned char MASK_CB    = 0x3f;
	
	// TODO add o_iterator_end
	template<typename T = ostream_iterator<char>>
	static size_t Encode(wchar_t codepoint, T& o_iterator, const T& o_end) {
        size_t encoded = 0;
        char buffer[4];
        size_t l = 1;
		if (codepoint <= L'\x7F') {
			// 1 byte
            l = 1;
			buffer[0] = static_cast<char>(codepoint);
		} else if (codepoint <= L'\x7FF') {
			// 2 byte
            l = 2;
            buffer[0] = (PREFIX_FB2) | (codepoint >> 6);
            buffer[1] = (PREFIX_CB)  | (codepoint & MASK_CB);
		} else if (codepoint <= L'\xFFFF') {
			// 3 byte
            l = 3;
            buffer[0] = (PREFIX_FB3) |  (codepoint >> 12);
            buffer[1] = (PREFIX_CB)  | ((codepoint >> 6) & MASK_CB);
            buffer[2] = (PREFIX_CB)  |  (codepoint       & MASK_CB);
		} else if (codepoint <= L'\x10FFFF') {
			// 4 byte
            l = 4;
            buffer[0] = (PREFIX_FB4) |  (codepoint >> 18);
            buffer[1] = (PREFIX_CB)  | ((codepoint >> 12) & MASK_CB);
            buffer[2] = (PREFIX_CB)  | ((codepoint >>  6) & MASK_CB);
            buffer[3] = (PREFIX_CB)  |  (codepoint        & MASK_CB);
		} else {
			throw "Illegal character codepoint";
		}
        for (encoded = 0; encoded < l; ++encoded, ++o_iterator) {
            if (o_iterator == o_end)
                throw "Output buffer to small";
            *o_iterator = buffer[encoded];
        }
        return encoded;
	}
	union ByteAttributes {
		uint16_t a {0};
		struct {
			unsigned isFirstByte        : 1;
			unsigned isContinuationByte : 1;
			unsigned isLegalByte        : 1;
			unsigned codePointLength    : 3;
			unsigned bits               : 7;
			unsigned bitLength          : 3;
		};
	};
	
	static ByteAttributes DecomposeByte(char byte) noexcept {
		// auto t = byte & PMASK_CB;
		// cout << "*" << static_cast<unsigned char>(byte) << "*";
		// cout << bitset<8>(byte) << "-" << bitset<8>(PMASK_FB1) << "-" << bitset<8>(byte & PMASK_FB1) << "-"
		//      << bitset<8>(PREFIX_FB1);
		// cout << sgn(byte & PMASK_FB1) << sgn(PREFIX_FB1);
		ByteAttributes ba;
		if ((byte & PMASK_CB) == PREFIX_CB) {
			ba.isContinuationByte = 1;
			ba.isLegalByte        = 1;
			ba.bits               = byte & MASK_CB;
			ba.bitLength          = 6;
		} 
		else if ((byte & PMASK_FB1) == PREFIX_FB1){
			ba.isFirstByte        = 1;
			ba.codePointLength    = 1;
			ba.isLegalByte        = 1;
			ba.bits               = byte & MASK_FB1;
			ba.bitLength          = 7;
		} 
		else if ((byte & PMASK_FB2) == PREFIX_FB2){
			ba.isFirstByte        = 1;
			ba.codePointLength    = 2;
			ba.isLegalByte        = 1;
			ba.bits               = byte & MASK_FB2;
			ba.bitLength          = 5;
		} 
		else if ((byte & PMASK_FB3) == PREFIX_FB3){
			ba.isFirstByte        = 1;
			ba.codePointLength    = 3;
			ba.isLegalByte        = 1;
			ba.bits               = byte & MASK_FB3;
			ba.bitLength          = 4;
		} 
		else if ((byte & PMASK_FB4) == PREFIX_FB4){
			ba.isFirstByte        = 1;
			ba.codePointLength    = 4;
			ba.isLegalByte        = 1;
			ba.bits               = byte & MASK_FB4;
			ba.bitLength          = 3;
		}
		return ba; // no legal first byte
	}
	static bool IsFirstByte(char byte) noexcept {
		return DecomposeByte(byte).isFirstByte == 1;
	}
	static bool IsContinuationByte(char byte) noexcept {
		return DecomposeByte(byte).isContinuationByte == 1;
	}
	static bool IsUTF8Byte(char byte) noexcept {
		return DecomposeByte(byte).isLegalByte == 1;
	}
	#define CHECKCONTINUATION(b) if (!IsContinuationByte(b)) {\
				throw "Illegal continuation octet";\
			}
	template<typename T = istream_iterator<char>>
	static void Decode(T& i_iterator, const T& i_end, wchar_t& codepoint) {
		wchar_t cp = 0;
		codepoint  = 0;
		if (i_iterator == i_end)
			return;
		for (size_t l = 1, maxl = 1; l <= maxl; ++l, ++i_iterator) {
			ByteAttributes ba = DecomposeByte(*i_iterator);
			if (!ba.isLegalByte)
				throw "Illegal octet";
			if (l == 1) {
				if (ba.isContinuationByte)
					throw "Continuation byte instead of first byte";
				maxl = ba.codePointLength;
			} else {
				if (ba.isFirstByte)
					throw "First byte instead of continuation byte";
			}
			cp <<= ba.bitLength;
            cp |= ba.bits;
			if (i_iterator == i_end)
				throw "EOF before codepoint completed";
		}
		codepoint = cp;
	}

	static size_t DecodeBuffer(const char* srcbuffer, size_t srclen, wchar_t* dstbuffer, size_t dstlen) {
		const char* srcit     = &srcbuffer[0];
		const char* srcend    = srcbuffer + srclen;
		wchar_t* dstit        = &dstbuffer[0];
		const wchar_t* dstend = dstbuffer + dstlen;
		size_t decoded        = 0;
		while(srcit != srcend && dstit != dstend) {
			Decode(srcit, srcend, *dstit);
			++decoded;
			++dstit;
		}
		if (dstbuffer != dstend) {
			*dstit = L'\0';
		}
		return decoded;
	}
	static size_t EncodeBuffer(const wchar_t* srcbuffer, size_t srclen, char* dstbuffer, size_t dstlen) {
		const wchar_t* srcit  = &srcbuffer[0];
		const wchar_t* srcend = srcbuffer + srclen;
		char* dstit           = &dstbuffer[0];
		char *const &dstend    = dstbuffer + dstlen;
		size_t encoded        = 0;
		while(srcit != srcend && dstit != dstend) {
			encoded += Encode(*srcit, dstit, dstend);
			++srcit;
		}
		if (dstbuffer != dstend) {
			*dstit = '\0';
		}
		return encoded;
	}
};

void test_ed() {
	string in(u8"Hällo");
	// array<char, 6> ain = {"Hallo"};
	cin >> in;
	wostringstream oss;
	ostream_iterator<wchar_t, wchar_t> oi(oss);
	for(auto it = in.begin(); it != in.end(); ++it) {
		wchar_t tmp;
		try {
			// UTF8::Decode(it, tmp);
			// *oi = tmp;
			auto b = *it;
			cout << static_cast<int>(b) << " " << b << " " << bitset<8>(b) << " " << bitset<16>(UTF8::DecomposeByte(b).a) << " ";
			if (UTF8::IsContinuationByte(b)) {
				cout << "Continuation" << endl;
			}
			else if (UTF8::IsFirstByte(b)) {
				cout << "First Byte of " 
				     << static_cast<unsigned>(UTF8::DecomposeByte(b).codePointLength) 
				     << " byte codepoint";
				wchar_t w = L'0';
				auto itt = it;
				UTF8::Decode(itt, in.end(), w); 
				wcout << w;
				cout << bitset<sizeof(wchar_t) * 8>(w) << endl;
			} 
			else {
				cout << "Illegal byte" << endl;
			}
		} catch(const char* e) {
			cout << "Exception: " << e << endl;
		}
		
	}
	wcout << "Ergebnis: " << oss.str() << endl;
	constexpr const size_t WBUFFERSIZE = 4096;
	constexpr const size_t CBUFFERSIZE = WBUFFERSIZE * 4;
	auto buffer = std::make_unique<wchar_t[]>(WBUFFERSIZE);
	wcout << "****************" << endl;
	auto decoded = UTF8::DecodeBuffer(in.data(), in.size(), buffer.get(), WBUFFERSIZE);
	wcout << "Decoded " << decoded << " wide characters" << endl;
	wcout << "Result: " << buffer.get() << "(" << std::wcslen(buffer.get()) << ")" << endl;
	auto cbuffer = std::make_unique<char[]>(CBUFFERSIZE);
	auto encoded = UTF8::EncodeBuffer(buffer.get(), decoded, cbuffer.get(), CBUFFERSIZE);
	cout << "Encoded " << encoded << " characters" << endl;
	std::string res(cbuffer.get());
	cout << "Result: " << res << "(" << res.size() << ")" << endl;
	cout << "in " << ((res == in)?"==":"!=") << " res" << endl;
}

int main(int argc, char const *argv[])
{
	// string test;
	// ostringstream oss(test);
	// ostream_iterator<char> osi(oss);
	// UTF8::Encode(L'\x20AC', osi);
	// UTF8::Encode(L'\xFFFE', osi);
	// UTF8::Encode(L'\x20AC', osi);
	// UTF8::Encode(L'\n', osi);
	// //cout << oss.str() << endl;
	// char buffer[10] = {0};
	// wctomb(buffer, L'\x20AC');
	// cout << buffer << endl;
	char a = 'A';
	cout << static_cast<int>(a) << " " << a << " " << bitset<8>(a) << " " << bitset<16>(UTF8::DecomposeByte(a).a) << endl;
	test_ed();
	return 0;
}