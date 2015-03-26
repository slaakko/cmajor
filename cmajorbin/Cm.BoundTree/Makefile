include ../Makefile.common

OBJECTS = BoundClass.o BoundCompileUnit.o BoundConcept.o BoundExpression.o BoundFunction.o BoundNode.o BoundStatement.o Visitor.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmboundtree.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmboundtree.a

