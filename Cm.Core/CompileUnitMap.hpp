/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_COMPILE_UNIT_MAP_INCLUDED
#define CM_CORE_COMPILE_UNIT_MAP_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Core {

class CompileUnitMap : public Cm::BoundTree::CompileUnitMap
{
public:
    void MapCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode, Cm::BoundTree::BoundCompileUnit* boundCompileUnit);
    Cm::BoundTree::BoundCompileUnit* GetBoundCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode);
    Cm::BoundTree::BoundCompileUnit* GetBoundCompileUnit(const std::string& compileUnitFilePath) override;
private:
    typedef std::unordered_map<Cm::Ast::CompileUnitNode*, Cm::BoundTree::BoundCompileUnit*> BoundMap;
    typedef BoundMap::const_iterator BoundMapIt;
    BoundMap boundMap;
    typedef std::unordered_map<std::string, Cm::Ast::CompileUnitNode*> CompileUnitNodeMap;
    typedef CompileUnitNodeMap::const_iterator CompileUnitNodeMapIt;
    CompileUnitNodeMap compileUnitNodeMap;
};

void SetCompileUnitMap(CompileUnitMap* compileUnitMap);

Cm::BoundTree::BoundCompileUnit* GetBoundCompileUnit(Cm::Ast::CompileUnitNode* compileUnitNode);

} } // namespace Cm::Core

#endif // CM_CORE_COMPILE_UNIT_MAP_INCLUDED
