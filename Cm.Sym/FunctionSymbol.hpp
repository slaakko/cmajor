/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_SYMBOL_INCLUDED
#define CM_SYM_FUNCTION_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.IrIntf/Fwd.hpp>

namespace Cm { namespace Sym {

class FunctionLookup
{
public:
    FunctionLookup(ScopeLookup lookup_, ContainerScope* scope_);
    ScopeLookup Lookup() const { return lookup; }
    void SetLookup(ScopeLookup lookup_) { lookup = lookup_; }
    ContainerScope* Scope() const { return scope; }
private:
    ScopeLookup lookup;
    ContainerScope* scope;
};

inline bool operator==(const FunctionLookup& left, const FunctionLookup& right)
{
    return left.Lookup() == right.Lookup() && left.Scope() == right.Scope();
}

class FunctionLookupSet
{
public:
    FunctionLookupSet();
    typedef std::vector<FunctionLookup>::const_iterator const_iterator;
    void Add(const FunctionLookup& lookup);
    const_iterator begin() const { return lookups.begin(); }
    const_iterator end() const { return lookups.end(); }
private:
    std::vector<FunctionLookup> lookups;
};

enum class FunctionSymbolFlags: uint16_t
{
    none = 0,
    constructorOrDestructorSymbol = 1 << 0,
    memberFunctionSymbol = 1 << 1,
    external = 1 << 2,
    cdecl_ = 1 << 3,
    virtual_ = 1 << 4,
    abstract_ = 1 << 5,
    override_ = 1 << 6,
    virtuality = virtual_ | abstract_ | override_,
    nothrow = 1 << 7,
    inline_ = 1 << 8,
    replicated = 1 << 9,
    suppressed = 1 << 10,
    default_ = 1 << 11,
    explicit_ = 1 << 12
};

inline FunctionSymbolFlags operator|(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint16_t(left) | uint16_t(right));
}

inline FunctionSymbolFlags operator&(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint16_t(left) & uint16_t(right));
}

class FunctionSymbol : public ContainerSymbol
{
public:
    FunctionSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::functionSymbol; }
    void SetFlags(FunctionSymbolFlags flags_) { flags = flags_; }
    const std::string& GroupName() const { return groupName; }
    void SetGroupName(const std::string& groupName_) { groupName = groupName_; }
    std::string TypeString() const override { return "function"; };
    bool IsFunctionSymbol() const override { return true; }
    virtual bool IsBasicTypeOp() const { return false; }
    virtual bool IsConvertingConstructor() const { return false; }
    bool IsConstructorOrDestructorSymbol() const { return GetFlag(FunctionSymbolFlags::constructorOrDestructorSymbol); }
    void SetConstructorOrDestructorSymbol() { SetFlag(FunctionSymbolFlags::constructorOrDestructorSymbol); }
    bool IsMemberFunctionSymbol() const { return GetFlag(FunctionSymbolFlags::memberFunctionSymbol); }
    void SetMemberFunctionSymbol() { SetFlag(FunctionSymbolFlags::memberFunctionSymbol); }
    bool IsExternal() const { return GetFlag(FunctionSymbolFlags::external); }
    void SetExternal() { SetFlag(FunctionSymbolFlags::external); }
    bool IsCDecl() const { return GetFlag(FunctionSymbolFlags::cdecl_); }
    void SetCDecl() { SetFlag(FunctionSymbolFlags::cdecl_); }
    bool IsAbstract() const { return GetFlag(FunctionSymbolFlags::abstract_); }
    bool IsOverride() const { return GetFlag(FunctionSymbolFlags::override_); }
    bool IsVirtualAbstractOrOverride() const { return GetFlag(FunctionSymbolFlags::virtuality); }
    void SetAbstract() { SetFlag(FunctionSymbolFlags::abstract_); }
    void SetVirtual() { SetFlag(FunctionSymbolFlags::virtual_);  }
    bool IsVirtual() const { return GetFlag(FunctionSymbolFlags::virtual_); }
    void SetOverride() { SetFlag(FunctionSymbolFlags::override_); }
    bool IsNothrow() const { return GetFlag(FunctionSymbolFlags::nothrow); }
    void SetNothrow() { SetFlag(FunctionSymbolFlags::nothrow); }
    bool IsInline() const { return GetFlag(FunctionSymbolFlags::inline_); }
    void SetInline() { SetFlag(FunctionSymbolFlags::inline_); }
    bool IsReplicated() const { return GetFlag(FunctionSymbolFlags::replicated); }
    void SetReplicated() { SetFlag(FunctionSymbolFlags::replicated); }
    bool IsSuppressed() const { return GetFlag(FunctionSymbolFlags::suppressed); }
    void SetSuppressed() { SetFlag(FunctionSymbolFlags::suppressed); }
    bool IsDefault() const { return GetFlag(FunctionSymbolFlags::default_); }
    void SetDefault() { SetFlag(FunctionSymbolFlags::default_); }
    bool IsExplicit() const { return GetFlag(FunctionSymbolFlags::explicit_); }
    void SetExplicit() { SetFlag(FunctionSymbolFlags::explicit_); }
    bool IsConstructor() const;
    bool IsDefaultConstructor() const;
    bool IsCopyConstructor() const;
    bool IsMoveConstructor() const;
    bool IsStaticConstructor() const;
    bool IsCopyAssignment() const;
    bool IsMoveAssignment() const;
    bool IsDestructor() const;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void AddSymbol(Symbol* symbol) override;
    void SetType(TypeSymbol* type_, int index) override;
    void SetReturnType(TypeSymbol* returnType_);
    TypeSymbol* GetReturnType() const { return returnType; }
    bool ReturnsClassObjectByValue() const;
    int Arity() const { return int(parameters.size()); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    void ComputeName();
    virtual TypeSymbol* GetTargetType() const;
    Cm::Ast::CompileUnitNode* CompileUnit() const { return compileUnit; }
    void SetCompileUnit(Cm::Ast::CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    void CollectExportedDerivedTypes(std::vector<TypeSymbol*>& exportedDerivedTypes) override;
    int16_t VtblIndex() const { return vtblIndex; }
    void SetVtblIndex(int16_t vtblIndex_) { vtblIndex = vtblIndex_; }
    Ir::Intf::Parameter* ClassObjectResultIrParam() const { return classObjectResultIrParam; }
    void SetClassObjectResultIrParam(Ir::Intf::Parameter* classObjectResultIrParam_) { classObjectResultIrParam = classObjectResultIrParam_; }
private:
    FunctionSymbolFlags flags;
    std::string groupName;
    int16_t vtblIndex;
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    Cm::Ast::CompileUnitNode* compileUnit;
    Ir::Intf::Parameter* classObjectResultIrParam;
    bool GetFlag(FunctionSymbolFlags flag) const
    {
        return (flags & flag) != FunctionSymbolFlags::none;
    }
    void SetFlag(FunctionSymbolFlags flag)
    {
        flags = flags | flag;
    }
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_SYMBOL_INCLUDED
