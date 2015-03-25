/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
#define CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
#include <Cm.BoundTree/BoundNode.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/DerivedTypeOpRepository.hpp>
#include <Cm.Core/EnumTypeOpRepository.hpp>
#include <Cm.Core/StringRepository.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.Core/SynthesizedClassFunRepository.hpp>
#include <Cm.Core/DelegateTypeOpRepository.hpp>
#include <Cm.Core/ClassDelegateTypeOpRepository.hpp>
#include <Cm.Core/FunctionTemplateRepository.hpp>
#include <Cm.Core/ConceptRepository.hpp>
#include <Cm.Core/ExternalConstantRepository.hpp>
#include <Cm.Core/InlineFunctionRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace BoundTree {

class BoundCompileUnit
{
public:
    BoundCompileUnit(Cm::Ast::CompileUnitNode* syntaxUnit_, const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_);
    Cm::Ast::CompileUnitNode* SyntaxUnit() const { return syntaxUnit; }
    void AddFileScope(Cm::Sym::FileScope* fileScope_);
    void RemoveLastFileScope();
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& GetFileScopes() const { return fileScopes; }
    const std::string& IrFilePath() const { return irFilePath; }
    const std::string& OptIrFilePath() const { return optIrFilePath; }
    const std::string& ObjectFilePath() const { return objectFilePath; }
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ConversionTable& ConversionTable() { return conversionTable; }
    Cm::Core::ClassConversionTable& ClassConversionTable() { return classConversionTable; }
    Cm::Core::DerivedTypeOpRepository& DerivedTypeOpRepository() { return derivedTypeOpRepository; }
    Cm::Core::EnumTypeOpRepository& EnumTypeOpRepository() {return enumTypeOpRepository; }
    Cm::Core::StringRepository& StringRepository() { return stringRepository; }
    Cm::Core::IrFunctionRepository& IrFunctionRepository() { return irFunctionRepository; }
    Cm::Core::IrClassTypeRepository& IrClassTypeRepository() { return irClassTypeRepository; }
    Cm::Core::ExternalConstantRepository& ExternalConstantRepository() { return externalConstantRepository; }
    void SetSynthesizedClassFunRepository(Cm::Core::SynthesizedClassFunRepository* synthesizedClassFunRepository_);
    Cm::Core::SynthesizedClassFunRepository& SynthesizedClassFunRepository() { return *synthesizedClassFunRepository; }
    void SetClassTemplateRepository(Cm::Core::ClassTemplateRepository* classTemplateRepository_);
    Cm::Core::ClassTemplateRepository& ClassTemplateRepository() { return *classTemplateRepository; }
    Cm::Core::FunctionTemplateRepository& FunctionTemplateRepository() { return functionTemplateRepository; }
    Cm::Core::InlineFunctionRepository& InlineFunctionRepository() { return *inlineFunctionRepository; }
    void SetInlineFunctionRepository(Cm::Core::InlineFunctionRepository* inlineFunctionRepository_);
    Cm::Core::DelegateTypeOpRepository& DelegateTypeOpRepository() { return *delegateTypeOpRepository; }
    void SetDelegateTypeOpRepository(Cm::Core::DelegateTypeOpRepository* delegateTypeOpRepository_);
    Cm::Core::ClassDelegateTypeOpRepository& ClassDelegateTypeOpRepository() { return *classDelegateTypeOpRepository; }
    void SetClassDelegateTypeOpRepository(Cm::Core::ClassDelegateTypeOpRepository* classDelegateTypeOpRepository_);
    Cm::Core::ConceptRepository& ConceptRepository() { return conceptRepository; }
    void AddBoundNode(BoundNode* boundNode);
    void Accept(Visitor& visitor);
    void Own(Cm::Sym::FunctionSymbol* functionSymbol);
    bool Instantiated(Cm::Sym::FunctionSymbol* functionSymbol) const;
    void AddToInstantiated(Cm::Sym::FunctionSymbol* functionSymbol);
    void SetHasGotos() { hasGotos = true; }
    bool HasGotos() const { return hasGotos; }
private:
    Cm::Ast::CompileUnitNode* syntaxUnit;
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
    std::string irFilePath;
    std::string objectFilePath;
    std::string optIrFilePath;
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ConversionTable conversionTable;
    Cm::Core::ClassConversionTable classConversionTable;
    Cm::Core::DerivedTypeOpRepository derivedTypeOpRepository;
    Cm::Core::EnumTypeOpRepository enumTypeOpRepository;
    Cm::Core::StringRepository stringRepository;
    Cm::Core::IrFunctionRepository irFunctionRepository;
    Cm::Core::IrClassTypeRepository irClassTypeRepository;
    Cm::Core::ExternalConstantRepository externalConstantRepository;
    std::unique_ptr<Cm::Core::SynthesizedClassFunRepository> synthesizedClassFunRepository;
    std::unique_ptr<Cm::Core::ClassTemplateRepository> classTemplateRepository;
    Cm::Core::FunctionTemplateRepository functionTemplateRepository;
    std::unique_ptr<Cm::Core::InlineFunctionRepository> inlineFunctionRepository;
    std::unique_ptr<Cm::Core::DelegateTypeOpRepository> delegateTypeOpRepository;
    std::unique_ptr<Cm::Core::ClassDelegateTypeOpRepository> classDelegateTypeOpRepository;
    Cm::Core::ConceptRepository conceptRepository;
    std::vector<std::unique_ptr<BoundNode>> boundNodes;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> ownedFunctionSymbols;
    std::unordered_set<Cm::Sym::FunctionSymbol*> instantiatedFunctions;
    bool hasGotos;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
