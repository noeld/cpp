CC=/Users/arnold/Documents/prg/clang/bin/clang
CXX=/Users/arnold/Documents/prg/clang/bin/clang++
RM=rm -f
CPPFLAGS=-U__STRICT_ANSI__ -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -I/Users/arnold/Documents/prg/libraries/cryptopp563/include -Wall -O2
LDFLAGS=-L/Users/arnold/Documents/prg/libraries/boost_1_56_0/stage/lib -L/Users/arnold/Documents/prg/libraries/cryptopp563/lib
LDLIBS=-lboost_system -lboost_filesystem -lcryptopp

SRCS=duplicate.cpp ThreadPool.cpp
OBJS=duplicate.o ThreadPool.o
OBJS=$(subst .cpp,.o,$(SRCS))

all: duplicate

#boost: $(OBJS)
#	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o boost $(OBJS) $(LDLIBS) 

duplicate: $(OBJS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o duplicate $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend

include .depend