/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

Cm::Sym::LocalVariableSymbol* BindLocalVariable(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::ConstructionStatementNode* constructionStatementNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(constructionStatementNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsLocalVariableSymbol())
        {
            Cm::Sym::LocalVariableSymbol* localVariableSymbol = static_cast<Cm::Sym::LocalVariableSymbol*>(symbol);
            if (localVariableSymbol->Bound())
            {
                return localVariableSymbol;
            }
            Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScope, constructionStatementNode->TypeExpr());
            localVariableSymbol->SetType(type);
            localVariableSymbol->SetBound();
            return localVariableSymbol;
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a local variable", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + constructionStatementNode->Id()->Str() + "' not found");
    }
}

} } // namespace Cm::Bind