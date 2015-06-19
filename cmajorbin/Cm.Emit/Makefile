include ../Makefile.common

OBJECTS = CEmitter.o CFunctionEmitter.o Emitter.o FunctionEmitter.o InitDone.o LlvmEmitter.o LlvmFunctionEmitter.o SourceFileCache.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmemit.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmemit.a

