/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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
    hasUserDefinedDestructor = 1 << 10,
    hasSuppressedDefaultConstructor = 1 << 11,
    hasSuppressedCopyConstructor = 1 << 12,
    hasSuppressedMoveConstructor = 1 << 13,
    hasSuppressedCopyAssignment = 1 << 14,
    hasSuppressedMoveAssignment = 1 << 15,
    generateDefaultConstructor = 1 << 16,
    generateCopyConstructor = 1 << 17,
    generateMoveConstructor = 1 << 18,
    generateCopyAssignment = 1 << 19,
    generateMoveAssignment = 1 << 20,
    generateDestructor = 1 << 21
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

struct PersistentClassData
{
    PersistentClassData();
    uint64_t classNodePos;
    uint64_t classNodeSize;
    std::string cmlFilePath;
    Cm::Ast::NodeList usingNodes;
    std::unique_ptr<Cm::Ast::ClassNode> classNode;
};

class ClassTypeSymbol : public TypeSymbol
{
public:
    ClassTypeSymbol(const Span& span_, const std::string& name_);
    ClassTypeSymbol(const Span& span_, const std::string& name_, bool getNextId);
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
    bool IsClassTemplateSymbol() const { return !typeParameters.empty(); }
    ClassTypeSymbol* BaseClass() const { return baseClass; }
    void SetBaseClass(ClassTypeSymbol* baseClass_) { baseClass = baseClass_; }
    void SetType(TypeSymbol* type, int index);
    bool HasBaseClass(ClassTypeSymbol* cls) const;
    bool HasBaseClass(ClassTypeSymbol* cls, int& distance) const;
    bool DoGenerateDestructor();
    bool DoGenerateStaticConstructor();
    bool HasNonTrivialMemberDestructor() const;
    void AddSymbol(Symbol* symbol) override;
    void AddConversion(FunctionSymbol* functionSymbol);
    const std::vector<MemberVariableSymbol*>& MemberVariables() const { return memberVariables; }
    const std::vector<MemberVariableSymbol*>& StaticMemberVariables() const { return staticMemberVariables; }
    void SetUsingNodes(const std::vector<Cm::Ast::Node*>& usingNodes_);
    const Cm::Ast::NodeList& GetUsingNodes() const;
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
    bool GenerateDestructor() const
    {
        return GetFlag(ClassTypeSymbolFlags::generateDestructor);
    }
    void SetGenerateDestructor()
    {
        SetFlag(ClassTypeSymbolFlags::generateDestructor);
    }
    FunctionSymbol* Destructor() const { return destructor; }
    FunctionSymbol* StaticConstructor() const { return staticConstructor; }
    const std::unordered_set<FunctionSymbol*> Conversions() const { return conversions; }
    MemberVariableSymbol* InitializedVar() const { return initializedVar.get(); }
    void SetInitializedVar(MemberVariableSymbol* initializedVar_);
    int16_t VPtrIndex() const { return vptrIndex; }
    void SetVPtrIndex(int16_t vptrIndex_) { vptrIndex = vptrIndex_; }
    ClassTypeSymbol* VPtrContainerClass() const;
    void InitVtbl();
    const std::vector<Cm::Sym::FunctionSymbol*>& Vtbl() const { return vtbl; }
    void InitVirtualFunctionTables() override;
    void MakeIrType() override;
    const std::vector<TypeParameterSymbol*>& TypeParameters() const { return typeParameters; }
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes) override;
private:
    ClassTypeSymbolFlags flags;
    ClassTypeSymbol* baseClass;
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
