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
#include <Cm.Core/SynthesizedClassFunRepository.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace BoundTree {

class BoundCompileUnit
{
public:
    BoundCompileUnit(Cm::Ast::CompileUnitNode* syntaxUnit_, const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_);
    Cm::Ast::CompileUnitNode* SyntaxUnit() const { return syntaxUnit; }
    void SetFileScope(Cm::Sym::FileScope* fileScope_);
    Cm::Sym::FileScope* GetFileScope() const { return fileScope.get(); }
    const std::string& IrFilePath() const { return irFilePath; }
    const std::string& ObjectFilePath() const { return objectFilePath; }
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ConversionTable& ConversionTable() { return conversionTable; }
    Cm::Core::ClassConversionTable& ClassConversionTable() { return classConversionTable; }
    Cm::Core::DerivedTypeOpRepository& DerivedTypeOpRepository() { return derivedTypeOpRepository; }
    Cm::Core::EnumTypeOpRepository& EnumTypeOpRepository() {return enumTypeOpRepository; }
    Cm::Core::StringRepository& StringRepository() { return stringRepository; }
    Cm::Core::IrFunctionRepository& IrFunctionRepository() { return irFunctionRepository; }
    Cm::Core::IrClassTypeRepository& IrClassTypeRepository() { return irClassTypeRepository; }
    void SetSynthesizedClassFunRepository(Cm::Core::SynthesizedClassFunRepository* synthesizedClassFunRepository_);
    Cm::Core::SynthesizedClassFunRepository& SynthesizedClassFunRepository() { return *synthesizedClassFunRepository; }
    void AddBoundNode(BoundNode* boundNode);
    void Accept(Visitor& visitor);
    void Own(Cm::Sym::FunctionSymbol* functionSymbol);
private:
    Cm::Ast::CompileUnitNode* syntaxUnit;
    std::unique_ptr<Cm::Sym::FileScope> fileScope;
    std::string irFilePath;
    std::string objectFilePath;
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ConversionTable conversionTable;
    Cm::Core::ClassConversionTable classConversionTable;
    Cm::Core::DerivedTypeOpRepository derivedTypeOpRepository;
    Cm::Core::EnumTypeOpRepository enumTypeOpRepository;
    Cm::Core::StringRepository stringRepository;
    Cm::Core::IrFunctionRepository irFunctionRepository;
    Cm::Core::IrClassTypeRepository irClassTypeRepository;
    std::unique_ptr<Cm::Core::SynthesizedClassFunRepository> synthesizedClassFunRepository;
    std::vector<std::unique_ptr<BoundNode>> boundNodes;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> ownedFunctionSymbols;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_COMPILE_UNIT_INCLUDED
