/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ConstExprFunctionRepository.hpp>

namespace Cm { namespace Bind {

ConstExprFunctionRepository::ConstExprFunctionRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

void ConstExprFunctionRepository::Release()
{
    for (Cm::Sym::FunctionSymbol* fun : constExprFunctions)
    {
        fun->FreeFunctionNode(boundCompileUnit.SymbolTable());
    }
}

Cm::Ast::Node* ConstExprFunctionRepository::GetNodeFor(Cm::Sym::FunctionSymbol* constExprFunctionSymbol)
{
    Cm::Ast::Node* node = boundCompileUnit.SymbolTable().GetNode(constExprFunctionSymbol, false);
    if (!node)
    {
        constExprFunctionSymbol->ReadFunctionNode(boundCompileUnit.SymbolTable(), constExprFunctionSymbol->GetSpan().FileIndex());
        node = boundCompileUnit.SymbolTable().GetNode(constExprFunctionSymbol);
        constExprFunctions.insert(constExprFunctionSymbol);
    }
    return node;
}

} } // namespace Cm::Bind
