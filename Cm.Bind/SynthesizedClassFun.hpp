/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_SYNTHESIZED_CLASS_FUN_INCLUDED
#define CM_BIND_SYNTHESIZED_CLASS_FUN_INCLUDED
#include <Cm.Core/Exception.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundClass.hpp>

namespace Cm { namespace Bind {

void GenerateSynthesizedFunctionImplementation(Cm::Sym::FunctionSymbol* function, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
    Cm::BoundTree::BoundCompileUnit& compileUnit, bool unique);
Cm::Sym::FunctionSymbol* GenerateDestructorSymbol(Cm::Sym::SymbolTable& symbolTable, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Ast::CompileUnitNode* compileUnit);
Cm::Sym::FunctionSymbol* GenerateStaticConstructorSymbol(Cm::Sym::SymbolTable& symbolTable, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Ast::CompileUnitNode* compileUnit);
void GenerateDestructorImplementation(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& compileUnit);
void GenerateStaticConstructorImplementation(Cm::BoundTree::BoundClass* boundClass, Cm::Sym::ContainerScope* containerScope, const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, 
    Cm::BoundTree::BoundCompileUnit& compileUnit);

class SynthesizedClassFunCache
{
public:
    SynthesizedClassFunCache();
    Cm::Sym::FunctionSymbol* GetDefaultConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* GetCopyConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* GetMoveConstructor(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* GetCopyAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* GetMoveAssignment(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope, 
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* GetOpEqual(const Cm::Parsing::Span& span, Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope,
        Cm::BoundTree::BoundCompileUnit& compileUnit, std::unique_ptr<Cm::Core::Exception>& exception);
    Cm::Sym::FunctionSymbol* DefaultCtor() const { return defaultConstructor.get(); }
    Cm::Sym::FunctionSymbol* CopyCtor() const { return copyConstructor.get(); }
    Cm::Sym::FunctionSymbol* MoveCtor() const { return moveConstructor.get(); }
    Cm::Sym::FunctionSymbol* CopyAssignment() const { return copyAssignment.get(); }
    Cm::Sym::FunctionSymbol* MoveAssignment() const { return moveAssignment.get(); }
    Cm::Sym::FunctionSymbol* OpEqual() const { return opEqual.get(); }
private:
    std::unique_ptr<Cm::Sym::FunctionSymbol> defaultConstructor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyConstructor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveConstructor;
    std::unique_ptr<Cm::Sym::FunctionSymbol> copyAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> moveAssignment;
    std::unique_ptr<Cm::Sym::FunctionSymbol> opEqual;
};

typedef std::unordered_map<Cm::Sym::ClassTypeSymbol*, SynthesizedClassFunCache> SynthesizedClassTypeCacheMap;
typedef SynthesizedClassTypeCacheMap::const_iterator SynthesizedClassTypeCacheMapIt;

class SynthesizedClassFunGroup
{
public:
    SynthesizedClassFunGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    virtual ~SynthesizedClassFunGroup();
    virtual void CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments, 
        const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception) = 0;
    Cm::BoundTree::BoundCompileUnit& CompileUnit() { return compileUnit; }
private:
    Cm::BoundTree::BoundCompileUnit& compileUnit;
};

class SynthesizedConstructorGroup : public SynthesizedClassFunGroup
{
public:
    SynthesizedConstructorGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, 
        Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception) override;
};

class SynthesizedAssignmentGroup : public SynthesizedClassFunGroup
{
public:
    SynthesizedAssignmentGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, 
        Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception) override;
};

class SynthesizedOpEqualGroup : public SynthesizedClassFunGroup
{
public:
    SynthesizedOpEqualGroup(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(SynthesizedClassTypeCacheMap& cacheMap, Cm::Sym::ClassTypeSymbol* classType, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span,
        Cm::Sym::ContainerScope* containerScope, std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception) override;
};

class SynthesizedClassFunRepository : public Cm::Core::SynthesizedClassFunRepository
{
public:
    SynthesizedClassFunRepository(Cm::BoundTree::BoundCompileUnit& compileUnit_);
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions, std::unique_ptr<Cm::Core::Exception>& exception) override;
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
    void AddDefaultFunctionSymbol(Cm::Sym::FunctionSymbol* defaultFunctionSymbol) override;
private:
    Cm::BoundTree::BoundCompileUnit& compileUnit;
    SynthesizedClassTypeCacheMap cacheMap;
    typedef std::unordered_map<std::string, SynthesizedClassFunGroup*> SynthesizedClassFunGroupMap;
    typedef SynthesizedClassFunGroupMap::const_iterator SynthesizedClassFunGroupMapIt;
    SynthesizedClassFunGroupMap synthesizedClassFunGroupMap;
    SynthesizedConstructorGroup synthesizedConstructorGroup;
    SynthesizedAssignmentGroup synthesizedAssignmentGroup;
    SynthesizedOpEqualGroup synthesizedOpEqualGroup;
    std::vector<Cm::Sym::FunctionSymbol*> defaultFunctionSymbols;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> ownedFunctionSymbols;
};

} } // namespace Cm::Bind

#endif // CM_BIND_SYNTHESIZED_CLASS_FUN_INCLUDED
