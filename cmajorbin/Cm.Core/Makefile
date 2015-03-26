include ../Makefile.common

OBJECTS = Argument.o BasicTypeOp.o ClassConversionTable.o ClassDelegateTypeOpRepository.o ClassTemplateRepository.o ConceptRepository.o \
DelegateTypeOpRepository.o DerivedTypeOpRepository.o EnumTypeOpRepository.o Exception.o ExternalConstantRepository.o \
FunctionTemplateRepository.o GenData.o GlobalSettings.o InitSymbolTable.o InlineFunctionRepository.o IrClassTypeRepository.o IrFunctionRepository.o \
StaticMemberVariableRepository.o StringRepository.o SynthesizedClassFunRepository.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmcore.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmcore.a

