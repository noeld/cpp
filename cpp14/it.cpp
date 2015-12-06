#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <string>
#include <functional>

using namespace std;
using namespace std::placeholders;

struct EH {
	size_t line {0};
	void handle_line() {
		cout << ++line << ". Zeile" << endl;
	}
};

template<typename T, typename IT>
void parse(T& eventhandler, IT& in, const IT& eof) {
	for(;in != eof; ++in) {
		cout << *in;
		if (*in == '\n')
			eventhandler.handle_line();
	}
}

int main (int argc, char const *argv[])
{
	int ret = 0;
	vector<string> params(argv, argv + argc);
	try {
		string filename = (params.size() >= 2)?params[1]:"it.txt";
		ifstream in(filename);
		in >> noskipws;
		if (in.good()) {
			istream_iterator<char> eof, it(in);
			EH eh;
			parse(eh, it, eof);
		} else {
			cerr << "Fehler beim Öffnen" << endl;
		}
	} catch(const exception& e) {
		cerr << "Ausnahme " << e.what() << endl;
	} catch(...) {
		cerr << "Unbekannte Ausnahme" << endl;
	}
	return ret;
}