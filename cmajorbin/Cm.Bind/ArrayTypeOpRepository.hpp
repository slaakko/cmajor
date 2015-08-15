/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
#define CM_BIND_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
#include <Cm.Core/ArrayTypeOpRepository.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>

namespace Cm { namespace Bind {

class ArrayTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetDefaultConstructor(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetCopyConstructor(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* type);
    Cm::Sym::FunctionSymbol* GetIndexing(Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit, Cm::Sym::TypeSymbol* arrayType);
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultConstructor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyConstructor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> indexing;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, ArrayTypeOpCache> ArrayTypeCacheMap;
typedef ArrayTypeCacheMap::const_iterator ArrayTypeCacheMapIt;

class ArrayTypeOpFunGroup
{
public:
    virtual ~ArrayTypeOpFunGroup();
    virtual void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
        const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class ArrayTypeConstructorGroup : public ArrayTypeOpFunGroup
{
public:
    void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
        const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ArrayTypeAssignmentGroup : public ArrayTypeOpFunGroup
{
public:
    void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, 
        const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ArrayTypeIndexGroup : public ArrayTypeOpFunGroup
{
public:
    void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit,
        const Cm::Parsing::Span& span, int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ArrayTypeOpRepository : public Cm::Core::ArrayTypeOpRepository
{
public:
    ArrayTypeOpRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
private:
    Cm::BoundTree::BoundCompileUnit& compileUnit;
    ArrayTypeCacheMap cacheMap;
    typedef std::unordered_map<std::string, ArrayTypeOpFunGroup*> ArrayTypeOpFunGroupMap;
    typedef ArrayTypeOpFunGroupMap::const_iterator ArrayTypeOpFunGroupMapIt;
    ArrayTypeOpFunGroupMap arrayTypeOpFunGroupMap;
    ArrayTypeConstructorGroup arrayTypeConstructorGroup;
    ArrayTypeAssignmentGroup arrayTypeAssignmentGroup;
    ArrayTypeIndexGroup arrayTypeIndexGroup;
};

} } // namespace Cm::Bind

#endif // CM_BIND_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
