/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

EnumTypeSymbol::EnumTypeSymbol(Cm::Ast::EnumTypeNode* enumTypeNode) : ContainerSymbol(enumTypeNode->Id()->Str())
{
    SetNode(enumTypeNode);
}

EnumConstantSymbol::EnumConstantSymbol(Cm::Ast::EnumConstantNode* enumConstantNode) : Symbol(enumConstantNode->Id()->Str())
{
    SetNode(enumConstantNode);
}

} } // namespace Cm::Sym