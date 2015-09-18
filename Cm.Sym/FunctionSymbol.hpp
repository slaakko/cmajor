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
#include <Cm.Sym/ReturnValueSymbol.hpp>
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

enum class ConversionType : uint8_t
{
    explicit_, implicit
};

enum class ConversionRank : uint8_t
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

enum class FunctionSymbolFlags: uint32_t
{
    none = 0,
    constructorOrDestructorSymbol = 1 << 0,
    memberFunctionSymbol = 1 << 1,
    cdecl_ = 1 << 2,
    virtual_ = 1 << 3,
    abstract_ = 1 << 4,
    override_ = 1 << 5,
    virtuality = virtual_ | abstract_ | override_,
    nothrow = 1 << 6,
    inline_ = 1 << 7,
    replicated = 1 << 8,
    suppressed = 1 << 9,
    default_ = 1 << 10,
    explicit_ = 1 << 11,
    conversion = 1 << 12,
    templateSpecialization = 1 << 13,
    memberOfTemplateType = 1 << 14,
    memberOfClassTemplate = 1 << 15,
    arrayConstructor = 1 << 16,
    arrayAssignment = 1 << 17,
    new_ = 1 << 18
};

std::string FunctionSymbolFlagString(FunctionSymbolFlags flags);

inline FunctionSymbolFlags operator|(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint32_t(left) | uint32_t(right));
}

inline FunctionSymbolFlags operator&(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint32_t(left) & uint32_t(right));
}

inline FunctionSymbolFlags operator~(FunctionSymbolFlags flag)
{
    return FunctionSymbolFlags(~uint32_t(flag));
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
    uint64_t functionPos;
    uint64_t functionSize;
    std::unique_ptr<Cm::Ast::FunctionNode> functionNode;
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
    virtual bool IsBasicTypeCopyMoveOrAssignmentOp() const { return false; }
    virtual bool IsBasicTypeCopyCtor() const { return false; }
    virtual bool IsDelegateFromFunCtor() const { return false; }
    virtual bool IsClassDelegateFromFunCtor() const { return false; }
    virtual bool IsDelegateFromFunAssignment() const { return false; }
    virtual bool IsClassDelegateFromFunAssignment() const { return false; }
    virtual bool IsConvertingConstructor() const;
    virtual bool IsConversionFunction() const;
    bool IsExportSymbol() const override;
    void SetConvertingConstructor();
    void SetConversionFunction();
    bool CheckIfConvertingConstructor() const;
    bool IsFunctionTemplate() const { return !typeParameters.empty(); }
    void SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_);
    const Cm::Ast::NodeList& GetUsingNodes() const;
    virtual ConversionType GetConversionType() const { return IsExplicit() ? Cm::Sym::ConversionType::explicit_ : Cm::Sym::ConversionType::implicit; }
    virtual ConversionRank GetConversionRank() const { return (IsConvertingConstructor() || IsConversionFunction()) ? Cm::Sym::ConversionRank::conversion : Cm::Sym::ConversionRank::exactMatch; }
    virtual int GetConversionDistance() const { return (IsConvertingConstructor() || IsConversionFunction()) ? 100 : 0; }
    bool IsFunctionTemplateSpecialization() const { return GetFlag(FunctionSymbolFlags::templateSpecialization); }
    void SetFunctionTemplateSpecialization() { SetFlag(FunctionSymbolFlags::templateSpecialization); }
    bool IsConstructorOrDestructorSymbol() const { return GetFlag(FunctionSymbolFlags::constructorOrDestructorSymbol); }
    void SetConstructorOrDestructorSymbol() { SetFlag(FunctionSymbolFlags::constructorOrDestructorSymbol); }
    bool IsMemberFunctionSymbol() const { return GetFlag(FunctionSymbolFlags::memberFunctionSymbol); }
    void SetMemberFunctionSymbol() { SetFlag(FunctionSymbolFlags::memberFunctionSymbol); }
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
    bool IsMemberOfClassTemplate() const { return GetFlag(FunctionSymbolFlags::memberOfClassTemplate); }
    void SetMemberOfClassTemplate() { SetFlag(FunctionSymbolFlags::memberOfClassTemplate); }
    bool IsArrayConstructor() const { return GetFlag(FunctionSymbolFlags::arrayConstructor); }
    void SetArrayConstructor() { SetFlag(FunctionSymbolFlags::arrayConstructor); }
    bool IsArrayAssignment() const { return GetFlag(FunctionSymbolFlags::arrayAssignment); }
    void SetArrayAssignment() { SetFlag(FunctionSymbolFlags::arrayAssignment); }
    bool IsNew() const { return GetFlag(FunctionSymbolFlags::new_); }
    void SetNew() { SetFlag(FunctionSymbolFlags::new_); }
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
    void ReadFunctionNode(Cm::Sym::SymbolTable& symbolTable, int fileIndex);
    void FreeFunctionNode(Cm::Sym::SymbolTable& symbolTable);
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
    void SetParameter(ParameterSymbol* parameter, int index) { parameters[index] = parameter; }
    ParameterSymbol* ThisParameter() const;
    ReturnValueSymbol* ReturnValue() const;
    void ComputeName();
    virtual TypeSymbol* GetTargetType() const;
    virtual TypeSymbol* GetSourceType() const;
    Cm::Ast::CompileUnitNode* CompileUnit() const { return compileUnit; }
    void SetCompileUnit(Cm::Ast::CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, TemplateTypeSymbol*, TypeIdHash>& exportedTemplateTypes) override;
    int16_t VtblIndex() const { return vtblIndex; }
    void SetVtblIndex(int16_t vtblIndex_) { vtblIndex = vtblIndex_; }
    Ir::Intf::Parameter* ClassObjectResultIrParam() const { return classObjectResultIrParam; }
    void SetClassObjectResultIrParam(Ir::Intf::Parameter* classObjectResultIrParam_) { classObjectResultIrParam = classObjectResultIrParam_; }
    void SetTypeArguments(const std::vector<Cm::Sym::TypeSymbol*>& typeArguments_) { typeArguments = typeArguments_; }
    void Dump(CodeFormatter& formatter) override;
    FileScope* GetFileScope(ContainerScope* containerScope);
    void SetGlobalNs(Cm::Ast::NamespaceNode* globalNs_);
    int GetMutexId() const { return mutexId; }
    void SetMutexId(int mutexId_) { mutexId = mutexId_; }
    void SetOverriddenFunction(FunctionSymbol* overriddenFunction_) { overriddenFunction = overriddenFunction_; }
    FunctionSymbol* OverriddenFunction() const { return overriddenFunction; }
    void SetFunctionTemplate(FunctionSymbol* functionTemplate_) { functionTemplate = functionTemplate_; }
    FunctionSymbol* FunctionTemplate() const { return functionTemplate; }
    std::unordered_set<FunctionSymbol*> OverrideSet() const { return overrideSet; }
    void AddToOverrideSet(FunctionSymbol* overrideFun);
    void SetConstraintDocId(const std::string& constraintDocId_);
    bool IsConst() const;
    std::string FullDocId() const override;
    std::string Syntax() const override;
    std::string ParsingName() const override;
    void ReplaceReplicaTypes() override;
    void DoSerialize() override;
private:
    FunctionSymbolFlags flags;
    std::string groupName;
    int16_t vtblIndex;
    TypeSymbol* returnType;
    ReturnValueSymbol* returnValueSymbol;
    std::vector<ParameterSymbol*> parameters;
    std::vector<TypeParameterSymbol*> typeParameters;
    std::vector<Cm::Sym::TypeSymbol*> typeArguments;
    Cm::Ast::CompileUnitNode* compileUnit;
    Ir::Intf::Parameter* classObjectResultIrParam;
    std::unique_ptr<PersistentFunctionData> persistentFunctionData;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs;
    int mutexId;
    FunctionSymbol* overriddenFunction;
    FunctionSymbol* functionTemplate;
    std::unordered_set<FunctionSymbol*> overrideSet;
    std::string constraintDocId;
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
