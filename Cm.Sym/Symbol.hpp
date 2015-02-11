/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_INCLUDED
#define CM_SYM_SYMBOL_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Cm.Ast/Specifier.hpp>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;
using Cm::Util::CodeFormatter;

class ContainerScope;
class NamespaceSymbol;
class ClassTypeSymbol;
class FunctionSymbol;
class ContainerSymbol;

enum class SymbolType : uint8_t
{
    boolSymbol, charSymbol, voidSymbol, sbyteSymbol, byteSymbol, shortSymbol, ushortSymbol, intSymbol, uintSymbol, longSymbol, ulongSymbol, floatSymbol, doubleSymbol, nullptrSymbol,
    classSymbol, constantSymbol, declarationBlock, delegateSymbol, classDelegateSymbol, enumTypeSymbol, enumConstantSymbol, functionSymbol, functionGroupSymbol, localVariableSymbol, 
    memberVariableSymbol, namespaceSymbol, parameterSymbol, typeParameterSymbol, templateTypeSymbol, derivedTypeSymbol, typedefSymbol, boundTypeParameterSymbol, conceptSymbol, conceptGroupSymbol,
    instantiatedConceptSymbol, 
    maxSymbol
};

std::string SymbolTypeStr(SymbolType st);

inline bool IsBasicSymbolType(SymbolType st)
{
    return st >= SymbolType::boolSymbol && st <= SymbolType::nullptrSymbol;
}

class Writer;
class Reader;
class TypeSymbol;

enum class SymbolSource
{
    project, library
};

enum class SymbolAccess : uint8_t
{
    private_ = 0, protected_ = 1, internal_ = 2, public_ = 3
};

std::string AccessStr(SymbolAccess access);

enum class SymbolFlags : uint8_t
{
    none = 0,
    access = 1 << 0 | 1 << 1,
    static_ = 1 << 2,
    bound = 1 << 3,
    project = 1 << 4
};

std::string SymbolFlagStr(SymbolFlags flags, SymbolAccess declaredAccess);

inline SymbolFlags operator~(SymbolFlags flag)
{
    return SymbolFlags(~uint8_t(flag));
}

inline SymbolFlags operator|(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint8_t(left) | uint8_t(right));
}

inline SymbolFlags operator&(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint8_t(left) & uint8_t(right));
}

class Symbol
{
public:
    Symbol(const Span& span_, const std::string& name_);
    virtual ~Symbol();
    virtual SymbolType GetSymbolType() const = 0;
    virtual void Write(Writer& writer);
    virtual void Read(Reader& reader);
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_) { name = name_; }
    std::string FullName() const;
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    SymbolAccess Access() const { return SymbolAccess(flags & SymbolFlags::access); }
    void SetAccess(SymbolAccess access_) { flags = flags | SymbolFlags(access_); }
    bool IsStatic() const { return GetFlag(SymbolFlags::static_); }
    void SetStatic() { SetFlag(SymbolFlags::static_); }
    virtual SymbolAccess DeclaredAccess() const { return Access(); }
    bool IsPublic() const { return Access() == SymbolAccess::public_; }
    void SetPublic() { SetAccess(SymbolAccess::public_); }
    SymbolSource Source() const { return GetFlag(SymbolFlags::project) ? SymbolSource::project : SymbolSource::library; }
    void SetSource(SymbolSource source) { if (source == SymbolSource::project) SetFlag(SymbolFlags::project); else ResetFlag(SymbolFlags::project); }
    Symbol* Parent() const { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    virtual std::string TypeString() const { return "symbol"; };
    virtual void SetType(TypeSymbol* typeSymbol, int index);
    virtual ContainerScope* GetContainerScope() const { return nullptr; }
    virtual bool IsExportSymbol() const;
    virtual bool WillBeExported() const;
    virtual bool IsNamespaceSymbol() const { return false; }
    virtual bool IsContainerSymbol() const { return false; }
    virtual bool IsTypeSymbol() const { return false; }
    virtual bool IsClassTypeSymbol() const { return false; }
    virtual bool IsConstantSymbol() const { return false; }
    virtual bool IsBasicTypeSymbol() const { return false; }
    virtual bool IsBoolTypeSymbol() const { return false; }
    virtual bool IsCharTypeSymbol() const { return false; }
    virtual bool IsEnumTypeSymbol() const { return false; }
    virtual bool IsVoidTypeSymbol() const { return false; }
    virtual bool IsIntegerTypeSymbol() const { return false; }
    virtual bool IsFloatingPointTypeSymbol() const { return false; }
    virtual bool IsDerivedTypeSymbol() const { return false; }
    virtual bool IsDelegateTypeSymbol() const { return false; }
    virtual bool IsClassDelegateTypeSymbol() const { return false; }
    virtual bool IsEnumConstantSymbol() const { return false; }
    virtual bool IsFunctionSymbol() const { return false; }
    virtual bool IsFunctionGroupSymbol() const { return false; }
    virtual bool IsConceptSymbol() const { return false; }
    virtual bool IsConceptGroupSymbol() const { return false; }
    virtual bool IsTemplateTypeSymbol() const { return false; }
    virtual bool IsParameterSymbol() const { return false; }
    virtual bool IsTypeParameterSymbol() const { return false; }
    virtual bool IsBoundTypeParameterSymbol() const { return false; }
    virtual bool IsMemberVariableSymbol() const { return false; }
    virtual bool IsLocalVariableSymbol() const { return false; }
    virtual bool IsTypedefSymbol() const { return false; }
    virtual bool IsNamespaceTypeSymbol() const { return false; }
    NamespaceSymbol* Ns() const;
    ClassTypeSymbol* Class() const;
    ContainerSymbol* ClassOrNs() const;
    ClassTypeSymbol* ContainingClass() const;
    FunctionSymbol* Function() const;
    FunctionSymbol* ContainingFunction() const;
    virtual bool IsSameParentOrAncestorOf(Symbol* that) const;
    bool Bound() const { return GetFlag(SymbolFlags::bound); }
    void SetBound() { SetFlag(SymbolFlags::bound); }
    SymbolFlags Flags() const { return flags; }
    bool GetFlag(SymbolFlags flag) const { return (flags & flag) != SymbolFlags::none;  }
    void SetFlag(SymbolFlags flag) { flags = flags | flag; }
    void ResetFlag(SymbolFlags flag) { flags = flags & ~flag; }
    virtual void Dump(CodeFormatter& formatter);
    virtual void CollectExportedDerivedTypes(std::vector<TypeSymbol*>& exportedDerivedTypes);
    virtual void InitVirtualFunctionTables();
    virtual void MakeIrType();
private:
    Span span;
    std::string name;
    SymbolFlags flags;
    Symbol* parent;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_INCLUDED
