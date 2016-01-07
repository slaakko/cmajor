all: build

build:
	$(MAKE) -C Cm.Util
	$(MAKE) -C Cm.Parsing.CppObjectModel
	$(MAKE) -C Cm.Parsing
	$(MAKE) -C Cm.Parsing.Cpp
	$(MAKE) -C Cm.Parsing.Syntax
	$(MAKE) -C Cm.Parsing.Generator
	$(MAKE) -C Cm.Ser
	$(MAKE) -C Ir.Intf
	$(MAKE) -C Dwarf
	$(MAKE) -C Llvm.Ir
	$(MAKE) -C C.Ir
	$(MAKE) -C Cm.Ast
	$(MAKE) -C Cm.Parser
	$(MAKE) -C Cm.IrIntf
	$(MAKE) -C Cm.Sym
	$(MAKE) -C Cm.Core
	$(MAKE) -C Cm.BoundTree
	$(MAKE) -C Cm.Opt
	$(MAKE) -C Cm.Bind
	$(MAKE) -C Cm.Emit
	$(MAKE) -C Cm.Build
	$(MAKE) -C Cm.Compiler
	$(MAKE) -C Sockets
	$(MAKE) -C Cm.Debugger
	$(MAKE) -C Cm.Profiler
	$(MAKE) -C Cm.Unit
	$(MAKE) -C cdidump
	$(MAKE) -C cmldump
	$(MAKE) -C SourceLines
	$(MAKE) -C StdHandleRedirector

prefix := /usr

install:
	mkdir -p $(prefix)/bin
	cp bin/* $(prefix)/bin

sys:
	cmc -backend=llvm -config=debug system/system.cms
	cmc -backend=llvm -config=release system/system.cms
	cmc -backend=llvm -config=profile system/system.cms
	cmc -backend=llvm -config=full system/system.cms
	cmc -backend=c -config=debug system/system.cms
	cmc -backend=c -config=release system/system.cms
	cmc -backend=c -config=profile system/system.cms
	cmc -backend=c -config=full system/system.cms

clean:
	$(MAKE) -C Cm.Util clean
	$(MAKE) -C Cm.Parsing.CppObjectModel clean
	$(MAKE) -C Cm.Parsing clean
	$(MAKE) -C Cm.Parsing.Cpp clean
	$(MAKE) -C Cm.Parsing.Syntax clean
	$(MAKE) -C Cm.Parsing.Generator clean
	$(MAKE) -C Cm.Ser clean
	$(MAKE) -C Ir.Intf clean
	$(MAKE) -C Dwarf clean
	$(MAKE) -C Llvm.Ir clean
	$(MAKE) -C C.Ir clean
	$(MAKE) -C Cm.Ast clean
	$(MAKE) -C Cm.Parser clean
	$(MAKE) -C Cm.IrIntf clean
	$(MAKE) -C Cm.Sym clean
	$(MAKE) -C Cm.Core clean
	$(MAKE) -C Cm.BoundTree clean
	$(MAKE) -C Cm.Opt clean
	$(MAKE) -C Cm.Bind clean
	$(MAKE) -C Cm.Emit clean
	$(MAKE) -C Cm.Build clean
	$(MAKE) -C Cm.Compiler clean
	$(MAKE) -C Sockets clean
	$(MAKE) -C Cm.Debugger clean
	$(MAKE) -C Cm.Profiler clean
	$(MAKE) -C Cm.Unit clean
	$(MAKE) -C cdidump clean
	$(MAKE) -C cmldump clean
	$(MAKE) -C SourceLines clean
	$(MAKE) -C StdHandleRedirector clean


