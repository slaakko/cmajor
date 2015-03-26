include ../Makefile.common

OBJECTS = BasicType.o Class.o Clone.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o Factory.o Function.o Identifier.o InitDone.o \
IntrinsicConcept.o Literal.o Namespace.o Node.o Parameter.o Project.o Reader.o Solution.o Specifier.o Statement.o SyntaxTree.o Template.o Typedef.o \
TypeExpr.o Visitor.o Writer.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmast.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmast.a

