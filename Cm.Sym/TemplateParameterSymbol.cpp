/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

TemplateParameterSymbol::TemplateParameterSymbol(Cm::Ast::TemplateParameterNode* templateParameterNode) : Symbol(templateParameterNode->Id()->Str())
{
    SetNode(templateParameterNode);
}

} } // namespace Cm::Sym