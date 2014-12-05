/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_ENUM_SYMBOL_INCLUDED
#define CM_SYM_ENUM_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Ast/Enumeration.hpp>

namespace Cm { namespace Sym {

class EnumTypeSymbol : public ContainerSymbol
{
public:
    EnumTypeSymbol(Cm::Ast::EnumTypeNode* enumTypeNode);
};

class EnumConstantSymbol : public Symbol
{
public:
    EnumConstantSymbol(Cm::Ast::EnumConstantNode* enumConstantNode);
};

} } // namespace Cm::Sym

#endif // CM_SYM_ENUM_SYMBOL_INCLUDED
