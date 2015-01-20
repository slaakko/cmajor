/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Util/Path.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace BoundTree {

BoundCompileUnit::BoundCompileUnit(Cm::Ast::CompileUnitNode* syntaxUnit_, const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_) : syntaxUnit(syntaxUnit_),
    fileScope(), irFilePath(irFilePath_), symbolTable(symbolTable_), conversionTable(symbolTable.GetStandardConversionTable()), classConversionTable(symbolTable.GetTypeRepository()), 
    derivedTypeOpRepository(symbolTable.GetTypeRepository()), enumTypeOpRepository(symbolTable.GetTypeRepository()), irFunctionRepository()
{
    objectFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".o").generic_string());
}

void BoundCompileUnit::SetFileScope(Cm::Sym::FileScope* fileScope_)
{
    fileScope.reset(fileScope_);
}

void BoundCompileUnit::AddBoundNode(BoundNode* boundNode)
{
    boundNodes.push_back(std::unique_ptr<BoundNode>(boundNode));
}

void BoundCompileUnit::Own(Cm::Sym::FunctionSymbol* functionSymbol)
{
    ownedFunctionSymbols.push_back(std::unique_ptr<Cm::Sym::FunctionSymbol>(functionSymbol));
}

void BoundCompileUnit::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        boundNode->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundCompileUnit::SetSynthesizedClassFunRepository(Cm::Core::SynthesizedClassFunRepository* synthesizedClassFunRepository_)
{
    synthesizedClassFunRepository.reset(synthesizedClassFunRepository_);
}

} } // namespace Cm::BoundTree
