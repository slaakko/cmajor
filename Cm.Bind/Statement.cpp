/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Statement.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Bind/Exception.hpp>

namespace Cm { namespace Bind {

StatementBinder::StatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_) : 
    ExpressionBinder(symbolTable_, containerScope_, fileScope_), symbolTable(symbolTable_), containerScope(containerScope_), fileScope(fileScope_), result(nullptr)
{
}

ConstructionStatementBinder::ConstructionStatementBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_) :
    StatementBinder(symbolTable_, containerScope_, fileScope_), constructionStatement(nullptr)
{
}

void ConstructionStatementBinder::BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    constructionStatement = new Cm::BoundTree::BoundConstructionStatement(&constructionStatementNode);
    constructionStatement->SetLocalVariable(BindLocalVariable(SymbolTable(), ContainerScope(), FileScope(), &constructionStatementNode));
}

void ConstructionStatementBinder::EndVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode)
{
    Cm::BoundTree::BoundExpressionList arguments = GetArguments();
    for (std::unique_ptr<Cm::BoundTree::BoundExpression>& argument : arguments)
    { 
        constructionStatement->AddArgument(argument.release());
    }
    SetResult(constructionStatement);
}

} } // namespace Cm::Bind