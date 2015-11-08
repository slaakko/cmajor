include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmutil.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=stdhandle_redirectord
else
    exename=stdhandle_redirector
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) \
-lcmutil -lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/$(exename) $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

