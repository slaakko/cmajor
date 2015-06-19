include ../Makefile.common

OBJECTS = main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmser.a ../lib/libirintf.a ../lib/libdwarf.a \
../lib/libcir.a ../lib/libllvmir.a ../lib/libcmast.a ../lib/libcmparser.a ../lib/libcmirintf.a ../lib/libcmsym.a ../lib/libcmcore.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmldumpd
else
    exename=cmldump
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) \
-lcmsym -lcmcore -lcmirintf -lcmparser -lcmast -lllvmir -lcir -ldwarf -lirintf -lcmser -lcmparsing -lcmparsingcppobjectmodel -lcmutil \
-lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/$(exename) $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

