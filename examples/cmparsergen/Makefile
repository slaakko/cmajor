all: llvm-version c-version

llvm-version:
	cmc -backend=llvm -config=release cmparsergen.cms

c-version:
	cmc -backend=c -config=release cmparsergen.cms

prefix := /usr

install-llvm:
	mkdir -p $(prefix)/bin
	cp Compiler/release/llvm/cmparsergen $(prefix)/bin

install-c:
	mkdir -p $(prefix)/bin
	cp Compiler/release/c/cmparsergen $(prefix)/bin

sources:
	$(MAKE) -C Cm sources
	$(MAKE) -C Syntax sources

