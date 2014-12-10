/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Sym {

ContainerSymbol::ContainerSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), containerScope()
{
    containerScope.SetContainer(this);
}

void ContainerSymbol::AddSymbol(Symbol* symbol)
{
    if (!symbol->Name().empty())
    {
        containerScope.Install(symbol);
    }
    symbols.push_back(std::unique_ptr<Symbol>(symbol));
    symbol->SetParent(this);
}

void ContainerSymbol::AddFunctionSymbol(FunctionSymbol* functionSymbol)
{
    symbols.push_back(std::unique_ptr<Symbol>(functionSymbol));
    functionSymbol->SetParent(this);
}

} } // namespace Cm::Sym
