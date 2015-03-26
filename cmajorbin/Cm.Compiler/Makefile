include ../Makefile.common

OBJECTS = IdeError.o Main.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmser.a ../lib/libirintf.a ../lib/libdwarf.a ../lib/libllvmir.a \
../lib/libcmast.a ../lib/libcmparser.a ../lib/libcmirintf.a ../lib/libcmsym.a ../lib/libcmcore.a ../lib/libcmboundtree.a ../lib/libcmbind.a ../lib/libcmemit.a ../lib/libcmbuild.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmcd
else
    exename=cmc
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) \
-lcmbuild -lcmemit -lcmbind -lcmboundtree -lcmcore -lcmsym -lcmirintf -lcmparser -lcmast -lllvmir -ldwarf -lirintf -lcmser -lcmparsing -lcmparsingcppobjectmodel -lcmutil \
-lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/$(exename) $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

