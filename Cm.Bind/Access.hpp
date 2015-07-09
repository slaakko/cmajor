/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_ACCESS_INCLUDED
#define CM_BIND_ACCESS_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Ast/Specifier.hpp>
#include <Cm.Parsing/Scanner.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

void SetAccess(Cm::Sym::Symbol* symbol, Cm::Ast::Specifiers specifiers, bool isClassMember);
void CheckAccess(Cm::Sym::Symbol* fromSymbol, const Span& fromSpan, Cm::Sym::Symbol* toSymbol);

} } // namespace Cm::Bind

#endif // CM_BIND_ACCESS_INCLUDED
