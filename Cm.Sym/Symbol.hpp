/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_INCLUDED
#define CM_SYM_SYMBOL_INCLUDED
#include <Cm.Sym/BoundCompileUnitSerialization.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <Cm.Ast/Specifier.hpp>
#include <Cm.Sym/TypeId.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;
using Cm::Util::CodeFormatter;

class ContainerScope;
class NamespaceSymbol;
class ClassTypeSymbol;
class InterfaceTypeSymbol;
class TemplateTypeSymbol;
class FunctionSymbol;
class ContainerSymbol;

enum class SymbolType : uint8_t
{
    boolSymbol, charSymbol, wcharSymbol, ucharSymbol, voidSymbol, sbyteSymbol, byteSymbol, shortSymbol, ushortSymbol, intSymbol, uintSymbol, longSymbol, ulongSymbol, floatSymbol, doubleSymbol, 
    nullptrSymbol,
    classSymbol, constantSymbol, declarationBlock, delegateSymbol, classDelegateSymbol, enumTypeSymbol, enumConstantSymbol, functionSymbol, functionGroupSymbol, localVariableSymbol, 
    memberVariableSymbol, namespaceSymbol, parameterSymbol, typeParameterSymbol, templateTypeSymbol, derivedTypeSymbol, typedefSymbol, boundTypeParameterSymbol, conceptSymbol, 
    conceptGroupSymbol, instantiatedConceptSymbol, functionGroupTypeSymbol, entrySymbol, returnValueSymbol,
    interfaceTypeSymbol, variableValueSymbol,
    maxSymbol
};

struct SymbolTypeHash
{
    size_t operator()(SymbolType x) const { return static_cast<size_t>(x); }
};

typedef std::unordered_set<SymbolType, SymbolTypeHash> SymbolTypeSet;
typedef SymbolTypeSet::const_iterator SymbolTypeSetIt;

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

enum class SymbolFlags : uint16_t
{
    none = 0,
    access = 1 << 0 | 1 << 1,
    static_ = 1 << 2,
    external = 1 << 3,
    bound = 1 << 4,
    project = 1 << 5,
    irTypeMade = 1 << 6,
    replica = 1 << 7,
    owned = 1 << 8,
    serialize = 1 << 9,
    justSymbol = 1 << 10
};

std::string SymbolFlagStr(SymbolFlags flags, SymbolAccess declaredAccess, bool addAccess);

inline SymbolFlags operator~(SymbolFlags flag)
{
    return SymbolFlags(~uint16_t(flag));
}

inline SymbolFlags operator|(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint16_t(left) | uint16_t(right));
}

inline SymbolFlags operator&(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint16_t(left) & uint16_t(right));
}

class Symbol;

class SymbolCollector
{
public:
    virtual ~SymbolCollector();
    virtual void Add(Symbol* symbol) = 0;
    virtual void EndContainer() = 0;
};

const uint32_t noSid = -1;

class Symbol : public BcuItem
{
public:
    Symbol(const Span& span_, const std::string& name_);
    virtual SymbolType GetSymbolType() const = 0;
    virtual void Write(Writer& writer);
    virtual void Read(Reader& reader);
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_) { name = name_; }
    virtual std::string FullName() const;
    virtual std::string FullCCName(SymbolTable& symbolTable) { return FullName(); }
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    SymbolAccess Access() const { return SymbolAccess(flags & SymbolFlags::access); }
    void SetAccess(SymbolAccess access_) { flags = flags | SymbolFlags(access_); }
    bool IsStatic() const { return GetFlag(SymbolFlags::static_); }
    void SetStatic() { SetFlag(SymbolFlags::static_); }
    bool IsExternal() const { return GetFlag(SymbolFlags::external); }
    void SetExternal() { SetFlag(SymbolFlags::external); }
    virtual SymbolAccess DeclaredAccess() const { return Access(); }
    SymbolAccess EffectiveAccess() const;
    bool IsPublic() const { return Access() == SymbolAccess::public_; }
    void SetPublic() { SetAccess(SymbolAccess::public_); }
    SymbolSource Source() const { return GetFlag(SymbolFlags::project) ? SymbolSource::project : SymbolSource::library; }
    void SetSource(SymbolSource source) { if (source == SymbolSource::project) SetFlag(SymbolFlags::project); else ResetFlag(SymbolFlags::project); }
    virtual Symbol* Parent() const { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    bool IsReplica() const { return GetFlag(SymbolFlags::replica); }
    void SetReplica() { SetFlag(SymbolFlags::replica); }
    bool IsProject() const { return GetFlag(SymbolFlags::project); }
    void SetProject() { SetFlag(SymbolFlags::project); }
    virtual std::string TypeString() const { return "symbol"; };
    virtual char CCTag() const { return 'S'; }
    virtual std::string CCName() const { return name; }
    virtual bool IsCCSymbol() const { return true; }
    virtual void SetType(TypeSymbol* typeSymbol, int index);
    virtual ContainerScope* GetContainerScope() const { return nullptr; }
    virtual bool IsExportSymbol() const;
    virtual bool IsVariableSymbol() const { return false; }
    virtual bool IsNamespaceSymbol() const { return false; }
    virtual bool IsContainerSymbol() const { return false; }
    virtual bool IsTypeSymbol() const { return false; }
    virtual bool IsClassTypeSymbol() const { return false; }
    virtual bool IsClassTemplateSymbol() const { return false; }
    virtual bool IsInterfaceTypeSymbol() const { return false; }
    virtual bool IsConstantSymbol() const { return false; }
    virtual bool IsBasicTypeSymbol() const { return false; }
    virtual bool IsBoolTypeSymbol() const { return false; }
    virtual bool IsCharTypeSymbol() const { return false; }
    virtual bool IsWCharTypeSymbol() const { return false; }
    virtual bool IsUCharTypeSymbol() const { return false; }
    virtual bool IsSByteTypeSymbol() const { return false; }
    virtual bool IsByteTypeSymbol() const { return false; }
    virtual bool IsShortTypeSymbol() const { return false; }
    virtual bool IsUShortTypeSymbol() const { return false; }
    virtual bool IsIntTypeSymbol() const { return false; }
    virtual bool IsUIntTypeSymbol() const { return false; }
    virtual bool IsLongTypeSymbol() const { return false; }
    virtual bool IsULongTypeSymbol() const { return false; }
    virtual bool IsFloatTypeSymbol() const { return false; }
    virtual bool IsDoubleTypeSymbol() const { return false; }
    virtual bool IsEnumTypeSymbol() const { return false; }
    virtual bool IsVoidTypeSymbol() const { return false; }
    virtual bool IsIntegerTypeSymbol() const { return false; }
    virtual bool IsCharacterTypeSymbol() const { return false; }
    virtual bool IsFloatingPointTypeSymbol() const { return false; }
    virtual bool IsDerivedTypeSymbol() const { return false; }
    virtual bool IsDelegateTypeSymbol() const { return false; }
    virtual bool IsClassDelegateTypeSymbol() const { return false; }
    virtual bool IsEnumConstantSymbol() const { return false; }
    virtual bool IsVariableValueSymbol() const { return false; }
    virtual bool IsFunctionSymbol() const { return false; }
    virtual bool IsFunctionGroupSymbol() const { return false; }
    virtual bool IsFunctionGroupTypeSymbol() const { return false; }
    virtual bool IsConceptSymbol() const { return false; }
    virtual bool IsConceptGroupSymbol() const { return false; }
    virtual bool IsTemplateTypeSymbol() const { return false; }
    virtual bool IsParameterSymbol() const { return false; }
    virtual bool IsTypeParameterSymbol() const { return false; }
    virtual bool IsBoundTypeParameterSymbol() const { return false; }
    virtual bool IsMemberVariableSymbol() const { return false; }
    virtual bool IsLocalVariableSymbol() const { return false; }
    virtual bool IsEntrySymbol() const { return false; }
    virtual bool IsReturnValueSymbol() const { return false; }
    virtual bool IsTypedefSymbol() const { return false; }
    virtual bool IsNamespaceTypeSymbol() const { return false; }
    virtual bool IsDeclarationBlock() const { return false; }
    NamespaceSymbol* Ns() const;
    ClassTypeSymbol* Class() const;
    InterfaceTypeSymbol* Interface() const;
    ContainerSymbol* ClassInterfaceOrNs() const;
    ClassTypeSymbol* ContainingClass() const;
    FunctionSymbol* Function() const;
    FunctionSymbol* ContainingFunction() const;
    virtual bool IsSameParentOrAncestorOf(Symbol* that) const;
    bool Bound() const { return GetFlag(SymbolFlags::bound); }
    void SetBound() { SetFlag(SymbolFlags::bound); }
    bool IrTypeMade() const { return GetFlag(SymbolFlags::irTypeMade); }
    void SetIrTypeMade() { SetFlag(SymbolFlags::irTypeMade); }
    void ResetIrTypeMade() { ResetFlag(SymbolFlags::irTypeMade); }
    SymbolFlags Flags() const { return flags; }
    bool GetFlag(SymbolFlags flag) const { return (flags & flag) != SymbolFlags::none;  }
    void SetFlag(SymbolFlags flag) { flags = flags | flag; }
    void ResetFlag(SymbolFlags flag) { flags = flags & ~flag; }
    virtual void Dump(CodeFormatter& formatter);
    virtual void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes);
    virtual void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes);
    virtual void InitVirtualFunctionTablesAndInterfaceTables();
    virtual void MakeIrType();
    virtual std::string DocId() const { return name; }
    virtual std::string FullDocId() const;
    virtual void Collect(SymbolCollector& collector);
    virtual std::string Syntax() const;
    virtual std::string ParsingName() const { return Name(); }
    void SetOwned() { SetFlag(SymbolFlags::owned); }
    void ResetOwned() { ResetFlag(SymbolFlags::owned); }
    bool Owned() const { return GetFlag(SymbolFlags::owned); }
    bool Serialize() const { return GetFlag(SymbolFlags::serialize); }
    virtual void DoSerialize();
    virtual void ReplaceReplicaTypes();
    void SetJustSymbol() { SetFlag(SymbolFlags::justSymbol); }
    bool JustSymbol() const { return GetFlag(SymbolFlags::justSymbol); }
    uint32_t Sid() const { return sid; }
    void SetSid(uint32_t sid_) { sid = sid_; }
    virtual TypeSymbol* GetType() const;
private:
    uint32_t sid;
    Span span;
    std::string name;
    SymbolFlags flags;
    Symbol* parent;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_INCLUDED
