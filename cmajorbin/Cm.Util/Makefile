include ../Makefile.common

OBJECTS = CodeFormatter.o MappedInputFile.o Mt.o Path.o Prime.o System.o TextUtils.o ToolError.o Uuid.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmutil.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmutil.a

