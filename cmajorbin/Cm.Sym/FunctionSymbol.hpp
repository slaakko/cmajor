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
#include <Cm.Ast/Concept.hpp>
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

enum class ConversionType
{
    explicit_, implicit
};

enum class ConversionRank
{
    exactMatch, promotion, conversion
};

inline bool BetterConversionRank(ConversionRank left, ConversionRank right)
{
    return left < right;
}

class FunctionLookupSet
{
public:
    FunctionLookupSet();
    typedef std::vector<FunctionLookup>::const_iterator const_iterator;
    void Add(const FunctionLookup& lookup);
    void Clear() { lookups.clear(); }
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
    explicit_ = 1 << 12,
    conversion = 1 << 13,
    templateSpecialization = 1 << 14,
    memberOfTemplateType = 1 << 15
};

std::string FunctionSymbolFlagString(FunctionSymbolFlags flags);

inline FunctionSymbolFlags operator|(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint16_t(left) | uint16_t(right));
}

inline FunctionSymbolFlags operator&(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint16_t(left) & uint16_t(right));
}

inline FunctionSymbolFlags operator~(FunctionSymbolFlags flag)
{
    return FunctionSymbolFlags(~uint16_t(flag));
}

class TypeParameterSymbol;

struct PersistentFunctionData
{
    PersistentFunctionData();
    uint64_t bodyPos;
    uint64_t bodySize;
    std::unique_ptr<Cm::Ast::Node> returnTypeExprNode;
    std::unique_ptr<Cm::Ast::FunctionGroupIdNode> groupId;
    std::unique_ptr<Cm::Ast::WhereConstraintNode> constraint;
    Cm::Ast::Specifiers specifiers;
    std::string cmlFilePath;
    Cm::Ast::NodeList usingNodes;
    std::unique_ptr<FileScope> functionFileScope;
};

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
    virtual bool IsDelegateFromFunCtor() const { return false; }
    virtual bool IsDelegateFromFunAssignment() const { return false; }
    virtual bool IsConvertingConstructor() const;
    bool IsExportSymbol() const override;
    void SetConvertingConstructor();
    bool CheckIfConvertingConstructor() const;
    bool IsFunctionTemplate() const { return !typeParameters.empty(); }
    void SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_);
    const Cm::Ast::NodeList& GetUsingNodes() const;
    virtual ConversionType GetConversionType() const { return IsExplicit() ? Cm::Sym::ConversionType::explicit_ : Cm::Sym::ConversionType::implicit; }
    virtual ConversionRank GetConversionRank() const { return IsConvertingConstructor() ? Cm::Sym::ConversionRank::conversion : Cm::Sym::ConversionRank::exactMatch; }
    virtual int GetConversionDistance() const { return IsConvertingConstructor() ? 100 : 0; }
    bool IsFunctionTemplateSpecialization() const { return GetFlag(FunctionSymbolFlags::templateSpecialization); }
    void SetFunctionTemplateSpecialization() { SetFlag(FunctionSymbolFlags::templateSpecialization); }
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
    void ResetNothrow() { ResetFlag(FunctionSymbolFlags::nothrow); }
    bool IsInline() const { return GetFlag(FunctionSymbolFlags::inline_); }
    bool CanThrow() const { return !IsCDecl() && !IsNothrow() && !IsDestructor(); }
    void SetInline() { SetFlag(FunctionSymbolFlags::inline_); }
    bool IsReplicated() const { return GetFlag(FunctionSymbolFlags::replicated); }
    void SetReplicated() { SetFlag(FunctionSymbolFlags::replicated); }
    bool IsSuppressed() const { return GetFlag(FunctionSymbolFlags::suppressed); }
    void SetSuppressed() { SetFlag(FunctionSymbolFlags::suppressed); }
    bool IsDefault() const { return GetFlag(FunctionSymbolFlags::default_); }
    void SetDefault() { SetFlag(FunctionSymbolFlags::default_); }
    bool IsExplicit() const { return GetFlag(FunctionSymbolFlags::explicit_); }
    void SetExplicit() { SetFlag(FunctionSymbolFlags::explicit_); }
    bool IsMemberOfTemplateType() const { return GetFlag(FunctionSymbolFlags::memberOfTemplateType); }
    void SetMemberOfTemplateType() { SetFlag(FunctionSymbolFlags::memberOfTemplateType); }
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
    Cm::Ast::Node* ReturnTypeExprNode() const { return persistentFunctionData->returnTypeExprNode.get(); }
    Cm::Ast::FunctionGroupIdNode* GroupId() const { return persistentFunctionData->groupId.get(); }
    bool HasConstraint() const { return persistentFunctionData && persistentFunctionData->constraint != nullptr; }
    Cm::Ast::WhereConstraintNode* Constraint() const { return persistentFunctionData->constraint.get(); }
    uint64_t BodyPos() const { return persistentFunctionData->bodyPos; }
    uint64_t BodySize() const { return persistentFunctionData->bodySize; }
    Cm::Ast::Specifiers GetSpecifiers() const { return persistentFunctionData->specifiers; }
    std::string CmlFilePath() const { return persistentFunctionData->cmlFilePath; }
    bool ReturnsClassObjectByValue() const;
    const std::vector<TypeParameterSymbol*>& TypeParameters() const { return typeParameters; }
    const std::vector<Cm::Sym::TypeSymbol*>& TypeArguments() const { return typeArguments; }
    int Arity() const { return int(parameters.size()); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    void ComputeName();
    virtual TypeSymbol* GetTargetType() const;
    Cm::Ast::CompileUnitNode* CompileUnit() const { return compileUnit; }
    void SetCompileUnit(Cm::Ast::CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    void CollectExportedDerivedTypes(std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes) override;
    int16_t VtblIndex() const { return vtblIndex; }
    void SetVtblIndex(int16_t vtblIndex_) { vtblIndex = vtblIndex_; }
    Ir::Intf::Parameter* ClassObjectResultIrParam() const { return classObjectResultIrParam; }
    void SetClassObjectResultIrParam(Ir::Intf::Parameter* classObjectResultIrParam_) { classObjectResultIrParam = classObjectResultIrParam_; }
    void SetTypeArguments(const std::vector<Cm::Sym::TypeSymbol*>& typeArguments_) { typeArguments = typeArguments_; }
    void Dump(CodeFormatter& formatter) override;
    FileScope* GetFileScope(ContainerScope* containerScope);
private:
    FunctionSymbolFlags flags;
    std::string groupName;
    int16_t vtblIndex;
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    std::vector<TypeParameterSymbol*> typeParameters;
    std::vector<Cm::Sym::TypeSymbol*> typeArguments;
    Cm::Ast::CompileUnitNode* compileUnit;
    Ir::Intf::Parameter* classObjectResultIrParam;
    std::unique_ptr<PersistentFunctionData> persistentFunctionData;
    bool GetFlag(FunctionSymbolFlags flag) const
    {
        return (flags & flag) != FunctionSymbolFlags::none;
    }
    void SetFlag(FunctionSymbolFlags flag)
    {
        flags = flags | flag;
    }
    void ResetFlag(FunctionSymbolFlags flag)
    {
        flags = flags & ~flag;
    }
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_SYMBOL_INCLUDED
