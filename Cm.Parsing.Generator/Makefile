include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmparsingcpp.a ../lib/libcmparsingsyntax.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../bin/cmpg: $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/cmpg $(OBJECTS) \
-lcmparsingsyntax -lcmparsingcpp -lcmparsing -lcmparsingcppobjectmodel -lcmutil -lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/cmpg $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/cmpg

