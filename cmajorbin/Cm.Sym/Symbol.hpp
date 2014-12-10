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
    boolSymbol, sbyteSymbol, byteSymbol, shortSymbol, ushortSymbol, intSymbol, uintSymbol, longSymbol, ulongSymbol, floatSymbol, doubleSymbol, charSymbol, voidSymbol, 
    classSymbol, constantSymbol, declarationBlock, delegateSymbol, classDelegateSymbol, enumTypeSymbol, enumConstantSymbol, functionSymbol, localVariableSymbol, memberVariableSymbol, namespaceSymbol, 
    parameterSymbol, templateParameterSymbol, derivedTypeSymbol,
    maxSymbol
};

const uint8_t firstBasicTypeSymbolType = uint8_t(SymbolType::boolSymbol);
const uint8_t lastBasicTypeSymbolType = uint8_t(SymbolType::voidSymbol);

class Symbol
{
public:
    Symbol(const Span& span_, const std::string& name_);
    virtual ~Symbol();
    virtual SymbolType GetSymbolType() const = 0;
    const std::string& Name() const { return name; }
    std::string FullName() const;
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    Symbol* Parent() const { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    virtual ContainerScope* GetContainerScope() { return nullptr; }
    virtual bool IsNamespaceSymbol() const { return false; }
    virtual bool IsClassSymbol() const { return false; }
    virtual bool IsConstantSymbol() const { return false; }
    virtual bool IsEnumTypeSymbol() const { return false; }
    virtual bool IsEnumConstantSymbol() const { return false; }
    NamespaceSymbol* Ns() const;
    ClassSymbol* Class() const;
private:
    std::string name;
    Span span;
    Symbol* parent;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_INCLUDED
