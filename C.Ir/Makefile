include ../Makefile.common

OBJECTS = Constant.o Factory.o Function.o Global.o Instruction.o Label.o MemberVar.o Parameter.o RefVar.o RegVar.o StackVar.o Type.o Util.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcir.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcir.a

