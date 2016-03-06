/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CLASS_SYMBOL_INCLUDED
#define CM_SYM_CLASS_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Sym {

class MemberVariableSymbol;
class TypeParameterSymbol;
class InterfaceTypeSymbol;

class ITable
{
public:
    ITable(InterfaceTypeSymbol* intf_);
    InterfaceTypeSymbol* Intf() const { return intf; }
    const std::vector<FunctionSymbol*>& IntfMemFunImpl() const { return intfMemFunImpl; }
    void SetImplementedMemFun(int index, FunctionSymbol* memFun);
private:
    InterfaceTypeSymbol* intf;
    std::vector<FunctionSymbol*> intfMemFunImpl;
};

enum class ClassTypeSymbolFlags : uint32_t
{
    none = 0,
    virtual_ = 1 << 0,
    abstract_ = 1 << 1,
    vtblInitialized = 1 << 2,
    hasUserDefinedConstructor = 1 << 3,
    hasUserDefinedDefaultConstructor = 1 << 4,
    hasUserDefinedCopyConstructor = 1 << 5,
    hasUserDefinedMoveConstructor = 1 << 6,
    hasUserDefinedStaticConstructor = 1 << 7,
    hasUserDefinedCopyAssignment = 1 << 8,
    hasUserDefinedMoveAssignment = 1 << 9,
    hasUserDefinedOpEqual = 1 << 10,
    hasUserDefinedDestructor = 1 << 11,
    hasSuppressedDefaultConstructor = 1 << 12,
    hasSuppressedCopyConstructor = 1 << 13,
    hasSuppressedMoveConstructor = 1 << 14,
    hasSuppressedCopyAssignment = 1 << 15,
    hasSuppressedMoveAssignment = 1 << 16,
    hasSuppressedOpEqual = 1 << 17,
    generateDefaultConstructor = 1 << 18,
    generateCopyConstructor = 1 << 19,
    generateMoveConstructor = 1 << 20,
    generateCopyAssignment = 1 << 21,
    generateMoveAssignment = 1 << 22,
    generateOpEqual = 1 << 23,
    generateDestructor = 1 << 24,
    debugInfoGenerated = 1 << 25,
    nonLeaf = 1 << 26,
    live = 1 << 27,
    baseClassSet = 1 << 28,
    implementedInterfacesSet = 1 << 29,
    itblsInitialized = 1 << 30
};

inline ClassTypeSymbolFlags operator&(ClassTypeSymbolFlags left, ClassTypeSymbolFlags right)
{
    return ClassTypeSymbolFlags(uint32_t(left) & uint32_t(right));
}

inline ClassTypeSymbolFlags operator|(ClassTypeSymbolFlags left, ClassTypeSymbolFlags right)
{
    return ClassTypeSymbolFlags(uint32_t(left) | uint32_t(right));
}

inline ClassTypeSymbolFlags operator~(ClassTypeSymbolFlags flag)
{
    return ClassTypeSymbolFlags(~uint32_t(flag));
}

bool Overrides(Cm::Sym::FunctionSymbol* f, Cm::Sym::FunctionSymbol* g);

struct PersistentClassData
{
    PersistentClassData();
    uint64_t classNodePos;
    uint64_t classNodeSize;
    std::string cmlFilePath;
    Cm::Ast::NodeList usingNodes;
    std::unique_ptr<Cm::Ast::ClassNode> classNode;
};

const uint64_t noCid = -1;

class ClassTypeSymbol : public TypeSymbol
{
public:
    ClassTypeSymbol(const Span& span_, const std::string& name_);
    ClassTypeSymbol(const Span& span_, const std::string& name_, bool getNextId, uint64_t cid_);
    ClassTypeSymbol(const Span& span_, const std::string& name_, const TypeId& id_);
    SymbolType GetSymbolType() const override { return SymbolType::classSymbol; }
    std::string TypeString() const override { return "class"; };
    std::string GetMangleId() const override;
    bool IsExportSymbol() const override;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void ReadClassNode(Cm::Sym::SymbolTable& symbolTable, int fileIndex);
    void FreeClassNode(Cm::Sym::SymbolTable& symbolTable);
    bool IsClassTypeSymbol() const override { return true; }
    bool IsClassTemplateSymbol() const override { return !typeParameters.empty(); }
    ClassTypeSymbol* BaseClass() const { return baseClass; }
    void SetBaseClass(ClassTypeSymbol* baseClass_) { baseClass = baseClass_; }
    const std::vector<InterfaceTypeSymbol*>& ImplementedInterfaces() const { return implementedInterfaces; }
    void AddImplementedInterface(InterfaceTypeSymbol* interfaceTypeSymbol);
    void SetType(TypeSymbol* type, int index);
    bool HasBaseClass(ClassTypeSymbol* cls) const;
    bool HasBaseClass(ClassTypeSymbol* cls, int& distance) const;
    bool DoGenerateDestructor();
    bool DoGenerateStaticConstructor();
    bool HasNonTrivialMemberDestructor() const;
    bool HasVirtualFunctions() const;
    void AddSymbol(Symbol* symbol) override;
    void AddConversion(FunctionSymbol* functionSymbol);
    const std::vector<MemberVariableSymbol*>& MemberVariables() const { return memberVariables; }
    const std::vector<MemberVariableSymbol*>& StaticMemberVariables() const { return staticMemberVariables; }
    void SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_);
    const Cm::Ast::NodeList& GetUsingNodes() const;
    const std::string& SourceFilePath() const { return sourceFilePath; }
    void SetSourceFilePath(const std::string& sourceFilePath_) { sourceFilePath = sourceFilePath_; }
    bool IsVirtual() const
    {
        return GetFlag(ClassTypeSymbolFlags::virtual_) || baseClass && baseClass->IsVirtual();
    }
    void SetVirtual()
    {
        SetFlag(ClassTypeSymbolFlags::virtual_);
    }
    bool IsAbstract() const override
    {
        return GetFlag(ClassTypeSymbolFlags::abstract_);
    }
    void SetAbstract()
    {
        SetFlag(ClassTypeSymbolFlags::abstract_);
    }
    bool HasUserDefinedConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedConstructor);
    }
    void SetHasUserDefinedConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedConstructor);
    }
    bool HasUserDefinedDefaultConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedDefaultConstructor);
    }
    void SetHasUserDefinedDefaultConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedDefaultConstructor);
    }
    bool HasUserDefinedCopyConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedCopyConstructor);
    }
    void SetHasUserDefinedCopyConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedCopyConstructor);
    }
    bool HasUserDefinedMoveConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedMoveConstructor);
    }
    void SetHasUserDefinedMoveConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedMoveConstructor);
    }
    bool HasUserDefinedStaticConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedStaticConstructor);
    }
    void SetHasUserDefinedStaticConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedStaticConstructor);
    }
    bool HasUserDefinedCopyAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedCopyAssignment);
    }
    void SetHasUserDefinedCopyAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedCopyAssignment);
    }
    bool HasUserDefinedMoveAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedMoveAssignment);
    }
    void SetHasUserDefinedMoveAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedMoveAssignment);
    }
    bool HasUserDefinedOpEqual() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedOpEqual);
    }
    void SetHasUserDefinedOpEqual()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedOpEqual);
    }
    bool HasUserDefinedDestructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasUserDefinedDestructor);
    }
    void SetHasUserDefinedDestructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasUserDefinedDestructor);
    }
    bool HasSuppressedDefaultConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedDefaultConstructor);
    }
    void SetHasSuppressedDefaultConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedDefaultConstructor);
    }
    bool HasSuppressedCopyConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedCopyConstructor);
    }
    void SetHasSuppressedCopyConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedCopyConstructor);
    }
    bool HasSuppressedMoveConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedMoveConstructor);
    }
    void SetHasSuppressedMoveConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedMoveConstructor);
    }
    bool HasSuppressedCopyAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedCopyAssignment);
    }
    void SetHasSuppressedCopyAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedCopyAssignment);
    }
    bool HasSuppressedMoveAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedMoveAssignment);
    }
    void SetHasSuppressedMoveAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedMoveAssignment);
    }
    bool HasSuppressedOpEqual() const
    {
        return GetFlag(ClassTypeSymbolFlags::hasSuppressedOpEqual);
    }
    void SetHasSuppressedOpEqual()
    {
        SetFlag(ClassTypeSymbolFlags::hasSuppressedOpEqual);
    }
    bool GenerateDefaultConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateDefaultConstructor);
    }
    void SetGenerateDefaultConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::generateDefaultConstructor);
    }
    bool GenerateCopyConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateCopyConstructor);
    }
    void SetGenerateCopyConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::generateCopyConstructor);
    }
    bool GenerateMoveConstructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateMoveConstructor);
    }
    void SetGenerateMoveConstructor()
    {
        SetFlag(ClassTypeSymbolFlags::generateMoveConstructor);
    }
    bool GenerateCopyAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateCopyAssignment);
    }
    void SetGenerateCopyAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::generateCopyAssignment);
    }
    bool GenerateMoveAssignment() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateMoveAssignment);
    }
    void SetGenerateMoveAssignment()
    {
        SetFlag(ClassTypeSymbolFlags::generateMoveAssignment);
    }
    bool GenerateOpEqual() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateOpEqual);
    }
    void SetGenerateOpEqual()
    {
        SetFlag(ClassTypeSymbolFlags::generateOpEqual);
    }
    bool GenerateDestructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateDestructor);
    }
    void SetGenerateDestructor()
    {
        SetFlag(ClassTypeSymbolFlags::generateDestructor);
    }
    bool DebugInfoGenerated() const 
    {
        return GetFlag(ClassTypeSymbolFlags::debugInfoGenerated);
    }
    void SetDebugInfoGenerated()
    {
        SetFlag(ClassTypeSymbolFlags::debugInfoGenerated);
    }
    bool IsNonLeaf() const
    {
        return GetFlag(ClassTypeSymbolFlags::nonLeaf);
    }
    void SetNonLeaf()
    {
        SetFlag(ClassTypeSymbolFlags::nonLeaf);
    }
    bool IsLive() const
    {
        return GetFlag(ClassTypeSymbolFlags::live);
    }
    void SetLive()
    {
        SetFlag(ClassTypeSymbolFlags::live);
    }
    bool BaseClassSet() const
    {
        return GetFlag(ClassTypeSymbolFlags::baseClassSet);
    }
    void SetBaseClassSet()
    {
        SetFlag(ClassTypeSymbolFlags::baseClassSet);
    }
    bool ImplementedInterfacesSet() const
    {
        return GetFlag(ClassTypeSymbolFlags::implementedInterfacesSet);
    }
    void SetImplementedInterfacesSet()
    {
        SetFlag(ClassTypeSymbolFlags::implementedInterfacesSet);
    }
    FunctionSymbol* Destructor() const { return destructor; }
    FunctionSymbol* StaticConstructor() const { return staticConstructor; }
    const std::unordered_set<FunctionSymbol*>& Conversions() const { return conversions; }
    MemberVariableSymbol* InitializedVar() const { return initializedVar.get(); }
    void SetInitializedVar(MemberVariableSymbol* initializedVar_);
    int16_t VPtrIndex() const { return vptrIndex; }
    void SetVPtrIndex(int16_t vptrIndex_) { vptrIndex = vptrIndex_; }
    ClassTypeSymbol* VPtrContainerClass() const;
    void InitVtbl();
    const std::vector<Cm::Sym::FunctionSymbol*>& Vtbl() const { return vtbl; }
    void InitVirtualFunctionTablesAndInterfaceTables() override;
    void InitItbls();
    const std::vector<ITable>& ITabs() const { return itabs; }
    void MakeIrType() override;
    const std::vector<TypeParameterSymbol*>& TypeParameters() const { return typeParameters; }
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes) override;
    Cm::Ast::CompileUnitNode* CompileUnit() const { return compileUnit; }
    void SetCompileUnit(Cm::Ast::CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    void Dump(CodeFormatter& formatter) override;
    std::string Syntax() const override;
    std::string FullClassTemplateId() const;
    void ReplaceReplicaTypes() override;
    uint64_t Cid() const { return cid; }
    void SetCid(uint64_t cid_) { cid = cid_; }
    uint64_t Key() const { return key; }
    void SetKey(uint64_t key_) { key = key_; }
    int Level() const { return level; }
    void SetLevel(int level_) { level = level_; }
    int Priority() const { return priority; }
    void SetPriority(int priority_) { priority = priority_; }
private:
    uint64_t cid;
    uint64_t key;
    int level;
    int priority;
    ClassTypeSymbolFlags flags;
    ClassTypeSymbol* baseClass;
    std::vector<InterfaceTypeSymbol*> implementedInterfaces;
    std::vector<ITable> itabs;
    std::vector<MemberVariableSymbol*> memberVariables;
    std::vector<MemberVariableSymbol*> staticMemberVariables;
    std::vector<TypeParameterSymbol*> typeParameters;
    FunctionSymbol* destructor;
    FunctionSymbol* staticConstructor;
    std::unique_ptr<MemberVariableSymbol> initializedVar;
    int16_t vptrIndex;
    std::vector<Cm::Sym::FunctionSymbol*> vtbl;
    std::unordered_set<FunctionSymbol*> conversions;
    std::unique_ptr<PersistentClassData> persistentClassData;
    std::string sourceFilePath;
    Cm::Ast::CompileUnitNode* compileUnit;
    bool GetFlag(ClassTypeSymbolFlags flag) const
    {
        return (flags & flag) != ClassTypeSymbolFlags::none;
    }
    void SetFlag(ClassTypeSymbolFlags flag)
    {
        flags = flags | flag;
    }
    void InitVtbl(std::vector<Cm::Sym::FunctionSymbol*>& vtblToInit);
};

} } // namespace Cm::Sym

#endif // CM_SYM_CLASS_SYMBOL_INCLUDED
