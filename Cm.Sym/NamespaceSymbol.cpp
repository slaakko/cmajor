/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>

namespace Cm { namespace Sym {

NamespaceSymbol::NamespaceSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

void NamespaceSymbol::Import(NamespaceSymbol* that, SymbolTable& symbolTable)
{
    symbolTable.BeginNamespaceScope(that->Name(), that->GetSpan());
    for (std::unique_ptr<Symbol>& ownedSymbol : that->Symbols())
    {
        if (ownedSymbol->IsNamespaceSymbol())
        {
            NamespaceSymbol* thatNs = static_cast<NamespaceSymbol*>(ownedSymbol.get());
            Import(thatNs, symbolTable);
        }
        else 
        {
            Symbol* symbol = ownedSymbol.release();
            if (symbol->IsFunctionSymbol())
            {
                FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
                symbolTable.Container()->AddFunctionSymbol(functionSymbol);
            }
            else
            {
                symbolTable.Container()->AddSymbol(symbol);
            }
        }
    }
    symbolTable.EndNamespaceScope();
}

} } // namespace Cm::Sym
