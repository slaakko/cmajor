include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmser.a ../lib/libirintf.a ../lib/libdwarf.a ../lib/libllvmir.a \
../lib/libcmast.a ../lib/libcmparser.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../bin/slines: $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/slines $(OBJECTS) \
-lcmparser -lcmast -lllvmir -ldwarf -lirintf -lcmser -lcmparsing -lcmparsingcppobjectmodel -lcmutil \
-lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/slines $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/slines

