include ../Makefile.common

OBJECTS = Access.o Binder.o Class.o ClassDelegateTypeOpRepository.o ClassObjectLayout.o ClassTemplateRepository.o Concept.o Constant.o \
ControlFlowAnalyzer.o Delegate.o DelegateTypeOpRepository.o Enumeration.o Evaluator.o ExpressionBinder.o Function.o InlineFunctionRepository.o \
LocalVariable.o MemberVariable.o OverloadResolution.o Parameter.o Prebinder.o StatementBinder.o SynthesizedClassFun.o Template.o Typedef.o \
TypeResolver.o VirtualBinder.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmbind.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmbind.a

