include ../Makefile.common

OBJECTS = Command.o DebugInfo.o Gdb.o GdbReply.o IdeInput.o IdeOutput.o InputReader.o Inspect.o InspectExpr.o InspectNode.o Interpreter.o \
Main.o Result.o Shell.o Type.o TypeExpr.o Util.o Value.o

LIBRARIES = ../lib/libcmutil.a ../lib/libcmparsingcppobjectmodel.a ../lib/libcmparsing.a ../lib/libcmser.a ../lib/libirintf.a ../lib/libdwarf.a ../lib/libllvmir.a \
../lib/libcir.a ../lib/libcmast.a ../lib/libcmparser.a ../lib/libcmirintf.a ../lib/libcmsym.a ../lib/libcmcore.a ../lib/libcmboundtree.a \
../lib/libcmbind.a ../lib/libcmemit.a ../lib/libcmbuild.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmdbd
else
    exename=cmdb
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) -pthread $(OBJECTS) \
-lcmbuild -lcmemit -lcmbind -lcmboundtree -lcmsym -lcmcore -lcmirintf -lcmparser -lcmast -lcir -lllvmir -ldwarf -lirintf -lcmser -lcmparsing -lcmparsingcppobjectmodel -lcmutil -lboost_filesystem -lboost_iostreams -lboost_system

install:
	mkdir -p $(prefix)/bin
	cp ../bin/$(exename) $(prefix)/bin

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

