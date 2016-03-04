include ../Makefile.common

OBJECTS = BasicTypeSymbol.o BoundCompileUnitSerialization.o ClassCounter.o ClassTypeSymbol.o ConceptGroupSymbol.o ConceptSymbol.o \
Conditional.o ConstantSymbol.o ContainerSymbol.o ConversionTable.o DeclarationBlock.o DeclarationVisitor.o DelegateSymbol.o \
DerivedTypeSymbol.o EntrySymbol.o EnumSymbol.o Exception.o ExceptionTable.o \
Factory.o FunctionGroupSymbol.o FunctionSymbol.o FunctionTable.o GlobalFlags.o InitDone.o InterfaceTypeSymbol.o LocalVariableSymbol.o \
MemberVariableSymbol.o Module.o MutexTable.o NameMangling.o NamespaceSymbol.o ParameterSymbol.o Reader.o ReturnValueSymbol.o Scope.o \
Symbol.o SymbolTable.o SymbolTypeSet.o TemplateTypeSymbol.o TypedefSymbol.o TypeId.o TypeParameterSymbol.o TypeRepository.o TypeSymbol.o \
Value.o VariableSymbol.o Warning.o Writer.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmsym.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmsym.a

