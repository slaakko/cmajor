/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
#define CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Template.hpp>

namespace Cm { namespace Sym {
    
class TemplateParameterSymbol : public Symbol
{
public:
    TemplateParameterSymbol(Cm::Ast::TemplateParameterNode* templateParameterNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_TEMPLATE_PARAMETER_SYMBOL_INCLUDED
