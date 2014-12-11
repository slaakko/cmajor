/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_INCLUDED
#define CM_SYM_SYMBOL_INCLUDED
#include <Cm.Parsing/Scanner.hpp>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;

class ContainerScope;
class NamespaceSymbol;
class ClassSymbol;

enum class SymbolType : uint8_t
{
    boolSymbol, charSymbol, voidSymbol, sbyteSymbol, byteSymbol, shortSymbol, ushortSymbol, intSymbol, uintSymbol, longSymbol, ulongSymbol, floatSymbol, doubleSymbol,
    classSymbol, constantSymbol, declarationBlock, delegateSymbol, classDelegateSymbol, enumTypeSymbol, enumConstantSymbol, functionSymbol, localVariableSymbol, memberVariableSymbol, namespaceSymbol, 
    parameterSymbol, templateParameterSymbol, templateTypeSymbol, typeSymbol, typedefSymbol,
    maxSymbol
};

class Writer;
class Reader;

enum class SymbolSource
{
    project, library
};

class Symbol
{
public:
    Symbol(const Span& span_, const std::string& name_);
    virtual ~Symbol();
    virtual SymbolType GetSymbolType() const = 0;
    virtual void Write(Writer& writer);
    virtual void Read(Reader& reader);
    const std::string& Name() const { return name; }
    std::string FullName() const;
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    SymbolSource Source() const { return source; }
    void SetSource(SymbolSource source_) { source = source_; }
    Symbol* Parent() const { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    virtual ContainerScope* GetContainerScope() { return nullptr; }
    virtual bool IsNamespaceSymbol() const { return false; }
    virtual bool IsTypeSymbol() const { return false; }
    virtual bool IsClassSymbol() const { return false; }
    virtual bool IsConstantSymbol() const { return false; }
    virtual bool IsBasicTypeSymbol() const { return false; }
    virtual bool IsBoolTypeSymbol() const { return false; }
    virtual bool IsCharTypeSymbol() const { return false; }
    virtual bool IsEnumTypeSymbol() const { return false; }
    virtual bool IsIntegerTypeSymbol() const { return false; }
    virtual bool IsFloatingPointTypeSymbol() const { return false; }
    virtual bool IsDerivedTypeSymbol() const { return false; }
    virtual bool IsDelegateTypeSymbol() const { return false; }
    virtual bool IsClassDelegateTypeSymbol() const { return false; }
    virtual bool IsEnumConstantSymbol() const { return false; }
    virtual bool IsFunctionSymbol() const { return false; }
    virtual bool IsParameterSymbol() const { return false; }
    virtual bool IsTypedefSymbol() const { return false; }
    NamespaceSymbol* Ns() const;
    ClassSymbol* Class() const;
    bool Bound() const { return bound; }
    void SetBound() { bound = true; }
private:
    std::string name;
    Span span;
    SymbolSource source;
    Symbol* parent;
    bool bound;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_INCLUDED
