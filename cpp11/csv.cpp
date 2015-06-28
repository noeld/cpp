/*
	Parse *.csv-files
*/
#include <iostream>
#include <regex>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

class CSVDefinition
{
public:
//	explicit CSVDefinition (arguments);	// type conversion
	CSVDefinition() = default;									// default ctor
	CSVDefinition(const CSVDefinition&) = default;							// copy ctor
	CSVDefinition(CSVDefinition&&) = default;								// move ctor
	CSVDefinition& operator=(const CSVDefinition&) = default;				// copy assignment
	CSVDefinition& operator=(CSVDefinition&&) = default;					// move assignment
	~CSVDefinition () {}
	
	const std::string& separator() const { return separator_; }
	std::string separator(const std::string& s) { std::string tmp = separator_; separator_ = s; return tmp; }
	
	const std::string& line_end() const { return line_end_; }
	std::string line_end(const std::string& l) { std::string tmp = line_end_; line_end_ = l; return tmp; }
	
	const std::string& string_delimiter() const { return string_delimiter_; } 
	std::string string_delimiter(const std::string& s) { std::string tmp = string_delimiter_; string_delimiter_ = s; return tmp; }

private:
	std::string separator_ {";"};
	std::string line_end_ {"\r\n"};
	std::string string_delimiter_ {"\""};
	uint32_t	max_line_length_ { 4096 };
} stddefinition;

class CSVFile
{
public:
	explicit CSVFile (std::istream& i, const CSVDefinition& d) : in_{i}, def_{d} {}	// type conversion
	CSVFile() = delete;									// default ctor
	CSVFile(const CSVFile&) = default;							// copy ctor
	CSVFile(CSVFile&&) = delete;								// move ctor
	CSVFile& operator=(const CSVFile&) = default;				// copy assignment
	CSVFile& operator=(CSVFile&&) = delete;					// move assignment
	~CSVFile () = default;

	std::vector<std::string> parse_line() {
		std::vector<std::string> tmp;
		char c;
		in_.get(c);
		std::string tval;
		bool instr {false};
		while(in_.good()) {
			switch(c) {
				case ';': 
					if (instr) {
						tval += c;
					} else {
						tmp.push_back(tval);
						tval.clear();
					}
					break;
				case '"':
					if (instr) {
						if (in_.peek() == '"') {
							in_.get(c);
							tval += c;
						} else {
							instr = false;
						}
					} else {
						instr = true;
					}
					break;
				case '\n':
				case '\r':
					//std::cerr << "\\n" << std::endl;
					if (instr) {
						tval += c;
					} else {
						tmp.push_back(tval);
						++line_;
						return tmp;
					}
					break;
				default:
					tval += c;
			}
			in_.get(c);
		}
		if (tval.length() > 0) {
			tmp.push_back(tval);
		}
		return tmp;
	}
	
	uint32_t line() const { return line_; }
	decltype(std::ifstream{""}.eof()) eof() { return in_.eof(); }
	
private:
	std::istream& in_;
	CSVDefinition def_;
	uint32_t	line_ {0};
};

class StoppWatch
{
public:
	StoppWatch() =default;									// default ctor
	StoppWatch(const StoppWatch&) =default;							// copy ctor
	StoppWatch& operator=(const StoppWatch&) =default;				// copy assignment
	virtual ~StoppWatch () {
		std::cerr << "Dauer: " 
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_).count()/1000.0
			<< " Sekunden" << std::endl;
	}

private:
	std::chrono::high_resolution_clock::time_point start_ { std::chrono::high_resolution_clock::now()};
} stoppwatch;

using namespace std;

int main (int argc, char const *argv[])
{
	int ret = 0;
	CSVDefinition d;
	if (argc < 2) {
		perror("Keine csv-Datei angegeben.");
		exit(1);
	}
	ifstream in(argv[1], ifstream::in | ifstream::binary);
	if (!in.good()) {
		cerr << "Kann Datei nicht öffnen." << endl;
		return -1;
	}
	stddefinition.separator(";");
	stddefinition.line_end("\n");
	CSVFile csvf(in, stddefinition);
	/*
	{	
		array<string, 3> a{{d.separator(), d.line_end(), d.string_delimiter()}};
		copy(a.begin(), a.end(), ostream_iterator<string>{cout});
	}*/
	while(!csvf.eof() && csvf.line() < 3) {
		auto fields = csvf.parse_line();
		cout << "*** Zeile: " << csvf.line() << endl;
		
		for(int i = 0; i < fields.size(); ++i) {
			cout << i << ": " << fields[i] << "(" << fields[i].length() << ")"<< endl;
		}
	}
	while(!csvf.eof()) {
		auto fields = csvf.parse_line();
		if (csvf.line() % 100000 == 0) {
			if (csvf.line() % 1000000 == 0) {
				cerr << csvf.line()/1000000 << 'M';
			} else {
				cerr << '.';
			}
		}
	}
	cout << "*** Zeile: " << csvf.line() << endl;
	return ret;
}