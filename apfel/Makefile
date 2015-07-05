CC=/Users/arnold/Documents/prg/clang/bin/clang
CXX=/Users/arnold/Documents/prg/clang/bin/clang++
RM=rm -f
CPPFLAGS=-g -std=c++14 -I/Users/arnold/Documents/prg/libraries/boost_1_56_0/ -I/Developer/SDKs/MacOSX10.6.sdk/usr/include -Wall -O0
LDFLAGS=-g
LDLIBS=

SRCS=apfel.cpp PicData.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend

include .depend