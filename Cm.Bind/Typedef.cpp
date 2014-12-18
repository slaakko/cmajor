/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Typedef.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Access.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::TypedefNode* typedefNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(typedefNode->Id()->Str());
    if (!symbol)
    {
        symbol = fileScope->Lookup(typedefNode->Id()->Str());
    }
    if (symbol)
    {
        if (symbol->IsTypedefSymbol())
        {
            Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
            BindTypedef(symbolTable, containerScope, fileScope, typedefNode, typedefSymbol);
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a typedef", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + typedefNode->Id()->Str() + "' not found");
    }
}

void BindTypedef(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::TypedefNode* typedefNode, Cm::Sym::TypedefSymbol* typedefSymbol)
{
    if (typedefSymbol->Evaluating())
    {
        throw Exception("cyclic typedef definitions detected", typedefSymbol->GetSpan());
    }
    if (typedefSymbol->Bound())
    {
        return;
    }
    Cm::Ast::Specifiers specifiers = typedefNode->GetSpecifiers();
    bool isClassMember = typedefNode->Parent()->IsClassNode();
    SetAccess(typedefSymbol, specifiers, isClassMember);
    typedefSymbol->SetEvaluating();
    bool willBeExported = typedefSymbol->WillBeExported();
    Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScope, typedefNode->TypeExpr(), willBeExported);
    typedefSymbol->ResetEvaluating();
    typedefSymbol->SetType(type);
    typedefSymbol->SetBound();
}

} } // namespace Cm::Bind