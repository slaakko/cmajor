/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Exception.hpp>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : globalNs(), currentScope(globalNs.GetScope())
{
    currentScope->SetNs(&globalNs);
}

void SymbolTable::BeginNamespaceScope(const std::string& qualifiedNsName)
{
    Symbol* symbol = currentScope->Lookup(qualifiedNsName);
    if (symbol)
    {
        if (symbol->IsNamespaceSymbol())
        {
            scopeStack.push(currentScope);
            currentScope = symbol->GetScope();
        }
        else
        {
            throw Exception("symbol '" + symbol->Name() + "' does not denote a namespace", symbol->GetNode(), nullptr);
        }
    }
    else
    {
        scopeStack.push(currentScope);
        currentScope = currentScope->CreateNamespace(qualifiedNsName);
    }
}

void SymbolTable::EndNamespaceScope()
{
    currentScope = scopeStack.top();
    scopeStack.pop();
}

} } // namespace Cm::Sym