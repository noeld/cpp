#include "PicData.h"
#include <stdio.h>
#include <string.h>

#define FP(f, name, typ, d) fprintf(f, #name " " typ "\n", d->name)

bool PicData::write(const string& filename) const {
	FILE* f = fopen(filename.c_str(), "w");
	if (NULL == f) {
		throw("PicData::write filename");
	}
	FP(f, max_it, "%d", this);
	FP(f, max_betrag, "%16.15e", this);
	FP(f, width, "%d", this);
	FP(f, height, "%d", this);
	FP(f, vx, "%16.15e", this);
	FP(f, vy, "%16.15e", this);
	FP(f, distance, "%16.15e", this);
	FP(f, rel, "%16.15e", this);
	FP(f, max_frames, "%d", this);
	FP(f, frame, "%d", this);
	FP(f, dist_factor, "%16.15e", this);
	fclose(f);
	return true;
}

#define CHECKDBL(dd, name, buffer, bufname, value) \
if (0 == strcmp(#name, bufname)) {\
dd->name =value;\
continue;\
}

#define CHECKUINT(dd, name, buffer, bufname, uvalue) \
if (0 == strcmp(#name, bufname)) {\
int ret = sscanf(buffer, "%s %u\n", bufname, &uvalue);\
if (EOF == ret) {\
perror("sscanf " #name);\
throw(string{"Format error with "} + string{#name});\
}\
dd->name = uvalue;\
continue;\
}

bool PicData::read(const string& filename) {
	FILE* f = fopen(filename.c_str(), "r");
	if (NULL == f) {
		string s{"Cannot open file "};
		s.append(filename);
		throw(s);
	}
	char buffer[512];
	while(NULL != fgets(buffer, 1024, f)) {
		char bufname[100];
		double value;
		unsigned uvalue;
		int ret = sscanf(buffer, "%s %lf\n", bufname, &value);
		if (EOF == ret) {
			throw(string{"Format error"});
		}
		CHECKUINT(this, max_it, buffer, bufname, uvalue)
		CHECKDBL(this, max_betrag, buffer, bufname, value)
		CHECKUINT(this, width, buffer, bufname, uvalue)
		CHECKUINT(this, height, buffer, bufname, uvalue)
		CHECKDBL(this, vx, buffer, bufname, value)
		CHECKDBL(this, vy, buffer, bufname, value)
		CHECKDBL(this, distance, buffer, bufname, value)
		CHECKDBL(this, rel, buffer, bufname, value)
		CHECKUINT(this, max_frames, buffer, bufname, uvalue)
		CHECKUINT(this, frame, buffer, bufname, uvalue)
		CHECKDBL(this, dist_factor, buffer, bufname, value)
		throw(string{"Unbekannter Parameter "} + string{bufname});
	}
	fclose(f);
	return true;
}

unsigned PicData::iter(const double& cx, const double& cy) const {
	double x = 0, xt = 0;
	double y = 0;
	double betrag = 0;
	unsigned it = 0;
	while( betrag <= this->max_betrag && it < this->max_it) {
		xt = x*x - y*y + cx;
		y = 2 * x * y + cy;
		x = xt;
		++it;
		betrag = x * x + y * y ;
	}
	if (it == this->max_it)
		it = 0;
	return it;
}