/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/CompileUnitMap.hpp>
#include <Cm.Sym/GlobalFlags.hpp>

namespace Cm { namespace Core {

void CompileUnitMap::MapCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode, Cm::BoundTree::BoundCompileUnit* boundCompileUnit)
{
    boundMap[compileUnitNode] = boundCompileUnit;
    compileUnitNodeMap[compileUnitNode->FilePath()] = compileUnitNode;
}

Cm::BoundTree::BoundCompileUnit* CompileUnitMap::GetBoundCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode)
{
    BoundMapIt i = boundMap.find(compileUnitNode);
    if (i != boundMap.end())
    {
        return i->second;
    }
    return nullptr;
}

Cm::BoundTree::BoundCompileUnit* CompileUnitMap::GetBoundCompileUnit(const std::string& compileUnitFilePath)
{
    CompileUnitNodeMapIt i = compileUnitNodeMap.find(compileUnitFilePath);
    if (i != compileUnitNodeMap.end())
    {
        return GetBoundCompileUnit(i->second);
    }
    return nullptr;
}

CompileUnitMap* globalCompileUnitMap = nullptr;

void SetCompileUnitMap(CompileUnitMap* compileUnitMap)
{
    globalCompileUnitMap = compileUnitMap;
    Cm::BoundTree::SetCompileUnitMap(compileUnitMap);
}

Cm::BoundTree::BoundCompileUnit* GetBoundCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode)
{
    return globalCompileUnitMap->GetBoundCompileUnit(compileUnitNode);
}

} } // namespace Cm::Core
