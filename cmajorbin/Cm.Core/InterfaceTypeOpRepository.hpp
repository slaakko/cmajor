/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_INTERFACE_TYPE_OP_REPOSITORY_INCLUDED
#define CM_CORE_INTERFACE_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/BasicTypeOp.hpp>
#include <Cm.Core/Argument.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <map>

namespace Cm { namespace Core {

class InterfaceObjectDefaultCtor : public BasicTypeOp
{
public:
    InterfaceObjectDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_);
    void Generate(Emitter& emitter, GenResult& result) override;
private:
    void GenerateLlvm(Emitter& emitter, GenResult& result);
    void GenerateC(Emitter& emitter, GenResult& result);
};

class InterfaceObjectCopyCtor : public BasicTypeOp
{
public:
    InterfaceObjectCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_);
    void Generate(Emitter& emitter, GenResult& result) override;
private:
    void GenerateLlvm(Emitter& emitter, GenResult& result);
    void GenerateC(Emitter& emitter, GenResult& result);
};

class InterfaceObjectCopyAssignment : public BasicTypeOp
{
public:
    InterfaceObjectCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_);
    void Generate(Emitter& emitter, GenResult& result) override;
private:
    void GenerateLlvm(Emitter& emitter, GenResult& result);
    void GenerateC(Emitter& emitter, GenResult& result);
};

class InterfaceObjectOpEqual : public BasicTypeOp
{
public:
    InterfaceObjectOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_);
    void Generate(Emitter& emitter, GenResult& result) override;
private:
    void GenerateLlvm(Emitter& emitter, GenResult& result);
    void GenerateC(Emitter& emitter, GenResult& result);
};

class InterfaceObjectFromClassPtrCtor : public BasicTypeOp
{
public:
    InterfaceObjectFromClassPtrCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType_, Cm::Sym::TypeSymbol* classPtrType_);
    void Generate(Emitter& emitter, GenResult& result) override;
    Cm::Sym::TypeSymbol* ClassPtrType() const { return classPtrType; }
    bool IsInterfaceObjectFromClassPtrCtor() const override { return true; }
private:
    Cm::Sym::TypeSymbol* classPtrType;
    void GenerateLlvm(Emitter& emitter, GenResult& result);
    void GenerateC(Emitter& emitter, GenResult& result);
};

class InterfaceTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetDefaultCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType);
    Cm::Sym::FunctionSymbol* GetCopyCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType);
    Cm::Sym::FunctionSymbol* GetInterfaceObjectFromClassPtrCtor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType, Cm::Sym::TypeSymbol* classPtrType);
    Cm::Sym::FunctionSymbol* GetOpEqual(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* interfaceType);
private:
    std::unique_ptr<InterfaceObjectDefaultCtor> defaultCtor;
    std::unique_ptr<InterfaceObjectCopyCtor> copyCtor;
    std::unique_ptr<InterfaceObjectCopyAssignment> copyAssignment;
    std::unique_ptr<InterfaceObjectFromClassPtrCtor> interfaceObjectFromClassPtrCtor;
    std::unique_ptr<InterfaceObjectOpEqual> opEqual;
};

typedef std::map<std::pair<Cm::Sym::TypeSymbol*, Cm::Sym::TypeSymbol*>, InterfaceTypeOpCache> InterfaceTypeOpCacheMap;

class InterfaceTypeOpGroup
{
public:
    virtual ~InterfaceTypeOpGroup();
    virtual void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository, 
        InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class InterfaceConstructorOpGroup : public InterfaceTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository, 
        InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class InterfaceAssignmentOpGroup : public InterfaceTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository,
        InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class InterfaceOpEqualGroup : public InterfaceTypeOpGroup
{
public:
    void CollectViableFunctions(int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::TypeRepository& typeRepository,
        InterfaceTypeOpCacheMap& interfaceTypeOpCacheMap, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class InterfaceTypeOpRepository
{
public:
    InterfaceTypeOpRepository(Cm::Sym::TypeRepository& typeRepository_);
    void CollectViableFunctions(Cm::Sym::ContainerScope* containerScope, const std::string& groupName, int arity, std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, 
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions);
private:
    Cm::Sym::TypeRepository& typeRepository;
    InterfaceConstructorOpGroup ctorOpGroup;
    InterfaceAssignmentOpGroup assignmentOpGroup;
    InterfaceOpEqualGroup opEqualGroup;
    std::unordered_map<std::string, InterfaceTypeOpGroup*> groupMap;
    InterfaceTypeOpCacheMap cacheMap;
};

} } // namespace Cm::Core

#endif // CM_CORE_INTERFACE_TYPE_OP_REPOSITORY_INCLUDED
