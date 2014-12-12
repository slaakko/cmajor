/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/MemberVariable.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Bind {

void BindMemberVariable(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* containerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::MemberVariableNode* memberVariableNode)
{
    Cm::Sym::Symbol* symbol = containerScope->Lookup(memberVariableNode->Id()->Str());
    if (symbol)
    {
        if (symbol->IsMemberVariableSymbol())
        {
            Cm::Sym::MemberVariableSymbol* memberVariableSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(symbol);
            if (memberVariableSymbol->Bound())
            {
                return;
            }
            Cm::Sym::TypeSymbol* type = ResolveType(symbolTable, containerScope, fileScope, TypeResolverTarget::memberVariable, memberVariableNode->TypeExpr());
            memberVariableSymbol->SetType(type);
            memberVariableSymbol->SetBound();
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a member variable", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + memberVariableNode->Id()->Str() + "' not found");
    }
}

} } // namespace Cm::Bind