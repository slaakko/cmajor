
Sources for Cmajor Compiler and Tools
=====================================

Cmajor is a hybrid programming language based on C++ and C#.  Its
syntax is closer to C#'s than C++'s syntax and semantics closer to
C++'s semantics than C#'s semantics.

It's not garbage collected but relies on destructors doing the cleanup
as in C++.

It runs on Windows and Linux, has LLVM and C backends, has basic
implementation of concepts, has an IDE in Windows, and has support for
debugging integrated in IDE in Windows and using command line tool
cmdb in Linux.

Latest release version can be found in [SourceForge Cmajor Site](http://sourceforge.net/projects/cmajor/).

Source code organization
------------------------

### Cmajor Compiler ###

+   **Cm.Compiler**

    Compiler Executable Project

+   **Cm.Ast**

    Abstract Syntax Tree

+   **Cm.Bind**

    Binder

+   **Cm.BoundTree**

    Bound Tree Representation

+   **Cm.Build**

    Build Driver

+   **Cm.Core**

    Function Repositories etc.

+   **Cm.Emit**

    Emitter

+   **Cm.IrIntf**

    Interface for Intermediate Code Generator Backends

+   **Cm.Opt**

    High Level Optimizer

+   **Cm.Parser**

    Parser

+   **Cm.Ser**

    Serialization Library

+   **Cm.Sym**

    Symbol Table

+   **Dwarf**

    Dwarf Debugging Constants (not used currently)

+   **Ir.Intf**

    Intermediate Code Interface

+   **Llvm.Ir**

    LLVM Code Generator

+    **C.Ir**

    C Code Generator

### Recursive Descent Backtracking Parser Generator Tool ###

+   **Cm.Parsing**

    Parsing Runtime Library

+   **Cm.Parsing.Cpp**

    Parser for C++ Semantic Actions

+   **Cm.Parsing.CppObjectModel**

    C++ Abstract Syntax Tree Classes

+   **Cm.Parsing.Generator**

    Parser Generator Executable Project

+   **Cm.Parsing.Syntax**

    Parser File Syntax

### Debugger ###

+   **Cm.Debugger**

    Debugger

### Profiler ###

+   **Cm.Profiler**

    Profiler

### Unit Test Engine ###

+   **Cm.Unit**

    Unit Tester

### Cmajor Libraries, Examples and Documentation ###

+   **system**

    Cmajor System Library

+   **examples**

    Cmajor Example Programs

+   **src**

    System Library Sources in PDF

+   **doc**

    Documentation

+   **test**

    Unit Tests for Compiler & Library

+   **unicode**

    Unicode Information Generator

### Utilities ###

+   **Cm.Util**

    Basic Utilities

+   **Sockets**

    Socket Interface

### Compiler Test Programs ###

+   **SourceLines**

    Source Code Line Statistic Calculator

+   **SourceRead**

    Perf Test
