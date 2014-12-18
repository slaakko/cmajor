/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundCompileUnit.hpp>

namespace Cm { namespace BoundTree {

BoundCompileUnit::BoundCompileUnit(Cm::Sym::SymbolTable& symbolTable_) : 
    symbolTable(symbolTable_), conversionTable(symbolTable.GetStandardConversionTable()), classConversionTable(symbolTable.GetTypeRepository())
{
}

void BoundCompileUnit::AddBoundNode(BoundNode* boundNode)
{
    boundNodes.push_back(std::unique_ptr<BoundNode>(boundNode));
}

} } // namespace Cm::BoundTree
