/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONTAINER_SYMBOL_INCLUDED
#define CM_SYM_CONTAINER_SYMBOL_INCLUDED
#include <Cm.Sym/Scope.hpp>

namespace Cm { namespace Sym {

class FunctionSymbol;

class ContainerSymbol : public Symbol
{
public:
    ContainerSymbol(const Span& span_, const std::string& name_);
    ContainerScope* GetContainerScope() override { return &containerScope; }
    void AddSymbol(Symbol* symbol);
    void AddFunctionSymbol(FunctionSymbol* functionSymbol);
private:
    ContainerScope containerScope;
    std::vector<std::unique_ptr<Symbol>> symbols;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONTAINER_SYMBOL_INCLUDED
