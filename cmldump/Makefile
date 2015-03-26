include ../Makefile.common

OBJECTS = main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmser.a ../lib/libirintf.a ../lib/libdwarf.a ../lib/libllvmir.a \
../lib/libcmast.a ../lib/libcmparser.a ../lib/libcmirintf.a ../lib/libcmsym.a ../lib/libcmcore.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../bin/cmldump: $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/cmldump $(OBJECTS) \
-lcmsym -lcmcore -lcmirintf -lcmparser -lcmast -lllvmir -ldwarf -lirintf -lcmser -lcmparsing -lcmparsingcppobjectmodel -lcmutil \
-lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/cmldump $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/cmldump

