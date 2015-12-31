/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#define CM_BIND_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>

namespace Cm { namespace Bind {

class ClassDelegateFromFunCtor : public Cm::Core::BasicTypeOp
{
public:
    ClassDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::ClassDelegateTypeSymbol* ClassDelegateType() const { return classDelegateType; }
    Cm::Sym::DelegateTypeSymbol* DelegateType() const { return delegateType; }
    Cm::Sym::FunctionSymbol* FunctionSymbol() const { return functionSymbol; }
    bool IsClassDelegateFromFunCtor() const override { return true; }
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuClassDelegateFromFunCtor; }
    void Write(Cm::Sym::BcuWriter& writer) override;
private:
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType;
    Cm::Sym::DelegateTypeSymbol* delegateType;
    Cm::Sym::FunctionSymbol* functionSymbol;
};

class ClassDelegateFromFunAssignment : public Cm::Core::BasicTypeOp
{
public:
    ClassDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::ClassDelegateTypeSymbol* ClassDelegateType() const { return classDelegateType; }
    Cm::Sym::DelegateTypeSymbol* DelegateType() const { return delegateType; }
    Cm::Sym::FunctionSymbol* FunctionSymbol() const { return functionSymbol; }
    bool IsClassDelegateFromFunAssignment() const override { return true; }
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuClassDelegateFromFunAssignment; }
    void Write(Cm::Sym::BcuWriter& writer) override;
private:
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType;
    Cm::Sym::DelegateTypeSymbol* delegateType;
    Cm::Sym::FunctionSymbol* functionSymbol;
};

class ClassDelegateEqualOp : public Cm::Sym::FunctionSymbol
{
public:
    ClassDelegateEqualOp(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType_);
    bool IsClassDelegateEqualOp() const override { return true; }
    Cm::Sym::BcuItemType GetBcuItemType() const override { return Cm::Sym::BcuItemType::bcuClassDelegateEqualOp; }
    void CreateBoundNode(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit);
    void Write(Cm::Sym::BcuWriter& writer) override;
private:
    Cm::Sym::ClassDelegateTypeSymbol* classDelegateType;
};

class ClassDelegateTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetClassDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, Cm::Sym::FunctionSymbol* fun);
    Cm::Sym::FunctionSymbol* GetClassDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType, Cm::Sym::FunctionSymbol* fun);
    Cm::Sym::FunctionSymbol* GetClassDelegateEqualOp(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ClassDelegateTypeSymbol* classDelegateType);
private:
    std::unordered_map<Cm::Sym::FunctionSymbol*, std::unique_ptr<Cm::Sym::FunctionSymbol>> classDelegateFromFunCtorMap;
    std::unordered_map<Cm::Sym::FunctionSymbol*, std::unique_ptr<Cm::Sym::FunctionSymbol>> classDelegateFromFunAssignmentMap;
    std::unique_ptr<Cm::Sym::FunctionSymbol> classDelegateEqualOp;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, ClassDelegateTypeOpCache>  ClassDelegateTypeOpCacheMap;

class ClassDelegateTypeOpGroup
{
public:
    virtual ~ClassDelegateTypeOpGroup();
    virtual void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& classDelegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class ClassDelegateConstructorOpGroup : public ClassDelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ClassDelegateAssignmentOpGroup : public ClassDelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ClassDelegateEqualOpGroup : public ClassDelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, ClassDelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ClassDelegateTypeOpRepository : public Cm::Core::ClassDelegateTypeOpRepository
{
public:
    ClassDelegateTypeOpRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    typedef std::unordered_map<std::string, ClassDelegateTypeOpGroup*> ClassDelegateTypeOpGroupMap;
    typedef ClassDelegateTypeOpGroupMap::const_iterator ClassDelegateTypeOpGroupMapIt;
    ClassDelegateTypeOpCacheMap classDelegateTypeOpCacheMap;
    ClassDelegateTypeOpGroupMap classDelegateTypeOpGroupMap;
    ClassDelegateConstructorOpGroup classDelegateConstructorOpGroup;
    ClassDelegateAssignmentOpGroup classDelegateAssignmentOpGroup;
    ClassDelegateEqualOpGroup classDelegateEqualOpGroup;
};

class ClassDelegateTypeOpFactory : public Cm::Sym::BcuClassDelegateTypeOpFactory
{
public:
    Cm::Sym::FunctionSymbol* CreateClassDelegateOp(Cm::Sym::BcuItemType itemType, Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol, 
        Cm::Sym::FunctionSymbol* functionSymbol) const override;
    Cm::Sym::FunctionSymbol* CreateClassDelegateOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::ClassDelegateTypeSymbol* classDelegateTypeSymbol) const override;
};

} } // namespace Cm::Bind

#endif  // CM_BIND_CLASS_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
