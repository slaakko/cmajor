include ../Makefile.common

OBJECTS = BasicType.o Class.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Error.o Expression.o FileRegistry.o Function.o Identifier.o \
Keyword.o Literal.o Parameter.o ParsingContext.o Project.o Solution.o Specifier.o Statement.o Template.o ToolError.o Typedef.o TypeExpr.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmparser.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

sources:
	cmpg Cm.Parser.pp

all_sources:
	cmpg -F Cm.Parser.pp

clean:
	rm -f *.o
	rm -f ../lib/libcmparser.a

