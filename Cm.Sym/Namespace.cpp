/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Namespace.hpp>

namespace Cm { namespace Sym {

NamespaceSymbol::NamespaceSymbol() : Symbol(), scope()
{
    scope.SetNs(this);
    scope.SetGlobal(&scope);
}

NamespaceSymbol::NamespaceSymbol(const std::string& name_) : Symbol(name_), scope()
{
    scope.SetNs(this);
}

void NamespaceSymbol::AddSymbol(Symbol* symbol)
{
    scope.Install(symbol);
    symbols.push_back(std::unique_ptr<Symbol>(symbol));
    symbol->SetParent(this);
}

} } // namespace Cm::Sym
