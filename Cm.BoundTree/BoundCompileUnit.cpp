/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Util/Path.hpp>
#include <boost/filesystem.hpp>

namespace Cm { namespace BoundTree {

BoundCompileUnit::BoundCompileUnit(const std::string& irFilePath_, Cm::Sym::SymbolTable& symbolTable_) : irFilePath(irFilePath_), symbolTable(symbolTable_), 
    conversionTable(symbolTable.GetStandardConversionTable()), classConversionTable(symbolTable.GetTypeRepository()), irFunctionRepository()
{
    objectFilePath = Cm::Util::GetFullPath(boost::filesystem::path(irFilePath).replace_extension(".o").generic_string());
}

void BoundCompileUnit::AddBoundNode(BoundNode* boundNode)
{
    boundNodes.push_back(std::unique_ptr<BoundNode>(boundNode));
}

void BoundCompileUnit::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        boundNode->Accept(visitor);
    }
}

} } // namespace Cm::BoundTree
