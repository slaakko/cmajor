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

namespace Cm { namespace Bind {

class ArrayTypeOpCache
{
public:
    Cm::Sym::FunctionSymbol* GetDefaultConstructor(Cm::Sym::TypeRepository& typeRepository, Cm::Sym::TypeSymbol* type);
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultConstructor;
};

typedef std::unordered_map<Cm::Sym::TypeSymbol*, ArrayTypeOpCache> ArrayTypeCacheMap;
typedef ArrayTypeCacheMap::const_iterator ArrayTypeCacheMapIt;

class ArrayTypeOpFunGroup
{
public:
    virtual ~ArrayTypeOpFunGroup();
    virtual void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span,
        int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
};

class ArrayTypeConstructorGroup : public ArrayTypeOpFunGroup
{
public:
    void CollectViableFunctions(ArrayTypeCacheMap& cacheMap, Cm::Sym::TypeSymbol* arrayType, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, const Cm::Parsing::Span& span,
        int arity, const std::vector<Cm::Core::Argument>& arguments, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
};

class ArrayTypeOpRepository : public Cm::Core::ArrayTypeOpRepository
{
public:
    ArrayTypeOpRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, 
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
private:
    Cm::BoundTree::BoundCompileUnit& compileUnit;
    ArrayTypeCacheMap cacheMap;
    typedef std::unordered_map<std::string, ArrayTypeOpFunGroup*> ArrayTypeOpFunGroupMap;
    typedef ArrayTypeOpFunGroupMap::const_iterator ArrayTypeOpFunGroupMapIt;
    ArrayTypeOpFunGroupMap arrayTypeOpFunGroupMap;
    ArrayTypeConstructorGroup arrayTypeConstructorGroup;
};

} } // namespace Cm::Bind

#endif // CM_BIND_ARRAY_TYPE_OP_REPOSITORY_INCLUDED
