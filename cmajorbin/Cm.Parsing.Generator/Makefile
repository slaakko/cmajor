include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmparsingcpp.a ../lib/libcmparsingsyntax.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmpgd
else
    exename=cmpg
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) \
-lcmparsingsyntax -lcmparsingcpp -lcmparsing -lcmparsingcppobjectmodel -lcmutil -lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/$(exename) $(prefix)/bin

all_sources:
	cmpg -F ../Cm.Parsing/StdLib.pp ../Cm.Parsing.Cpp/Cm.Parsing.Cpp.pp ../Cm.Parsing.Syntax/Cm.Parsing.Syntax.pp

sources:
	cmpg ../Cm.Parsing/StdLib.pp ../Cm.Parsing.Cpp/Cm.Parsing.Cpp.pp ../Cm.Parsing.Syntax/Cm.Parsing.Syntax.pp

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

