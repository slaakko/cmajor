/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#define CM_BIND_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Core/DelegateTypeOpRepository.hpp>
#include <Cm.Core/BasicTypeOp.hpp>

namespace Cm { namespace Bind {

class DelegateFromFunCtor : public Cm::Core::BasicTypeOp
{
public:
    DelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType_, Cm::Sym::DelegateTypeSymbol* delegateType_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::DelegateTypeSymbol* DelegateType() const { return delegateType; }
    Cm::Sym::FunctionSymbol* FunctionSymbol() const { return functionSymbol; }
    bool IsDelegateFromFunCtor() const override { return true; }
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
private:
    Cm::Sym::DelegateTypeSymbol* delegateType;
    Cm::Sym::FunctionSymbol* functionSymbol;
};

class DelegateFromFunAssignment : public Cm::Core::BasicTypeOp
{
public:
    DelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType_, Cm::Sym::DelegateTypeSymbol* delegateType_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::DelegateTypeSymbol* DelegateType() const { return delegateType; }
    Cm::Sym::FunctionSymbol* FunctionSymbol() const { return functionSymbol; }
    bool IsDelegateFromFunAssignment() const override { return true; }
    void Generate(Cm::Core::Emitter& emitter, Cm::Core::GenResult& result) override;
private:
    Cm::Sym::DelegateTypeSymbol* delegateType;
    Cm::Sym::FunctionSymbol* functionSymbol;
};

class DelegateTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetMoveCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetMoveAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetOpLess(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegateType);
    Cm::Sym::FunctionSymbol* GetDelegateFromFunCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType, Cm::Sym::DelegateTypeSymbol* delegateType, 
        Cm::Sym::FunctionSymbol* fun);
    Cm::Sym::FunctionSymbol* GetDelegateFromFunAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* delegatePtrType, Cm::Sym::DelegateTypeSymbol* delegateType, 
        Cm::Sym::FunctionSymbol* fun);
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opEqual;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opLess;
    std::unique_ptr<Cm::Sym::FunctionSymbol> delegateFromFunCtor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> delegateFromFunAssignment;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, DelegateTypeOpCache>  DelegateTypeOpCacheMap;

class DelegateTypeOpGroup
{
public:
    virtual ~DelegateTypeOpGroup();
    virtual void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span, 
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class DelegateConstructorOpGroup : public DelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DelegateAssignmentOpGroup : public DelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DelegateEqualOpGroup : public DelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DelegateLessOpGroup : public DelegateTypeOpGroup
{
public:
    void CollectViableFunctions(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        Cm::Sym::TypeRepository& typeRepository, DelegateTypeOpCacheMap& delegateTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class DelegateTypeOpRepository : public Cm::Core::DelegateTypeOpRepository
{
public:
    DelegateTypeOpRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName, 
        int arity, std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ConversionTable& conversionTable,
        const Cm::Parsing::Span& span, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    typedef std::unordered_map<std::string, DelegateTypeOpGroup*> DelegateTypeOpGroupMap;
    typedef DelegateTypeOpGroupMap::const_iterator DelegateTypeOpGroupMapIt;
    DelegateTypeOpCacheMap delegateTypeOpCacheMap;
    DelegateTypeOpGroupMap delegateTypeOpGroupMap;
    DelegateConstructorOpGroup delegateConstructorOpGroup;
    DelegateAssignmentOpGroup delegateAssignmentOpGroup;
    DelegateEqualOpGroup delegateEqualOpGroup;
    DelegateLessOpGroup delegateLessOpGroup;

};

} } // namespace Cm::Bind

#endif // CM_BIND_DELEGATE_TYPE_OP_REPOSITORY_INCLUDED
