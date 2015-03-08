/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/Visitor.hpp>

namespace Cm { namespace BoundTree {

BoundFunction::BoundFunction(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_) : BoundNode(syntaxNode_), functionSymbol(functionSymbol_), classObjectLayoutFunIndex(0), hasGotos(false),
    nextCatchId(0)
{
}

void BoundFunction::SetBody(BoundCompoundStatement* body_)
{
    body.reset(body_);
}

void BoundFunction::AddLocalVariable(Cm::Sym::LocalVariableSymbol* localVariable)
{
    localVariables.push_back(localVariable);
}

std::string BoundFunction::GetNextTempVariableName()
{
    std::string tempVarName = "__temp" + std::to_string(int(temporaries.size()));
    temporaries.push_back(nullptr);
    return tempVarName;
}

Cm::Sym::LocalVariableSymbol* BoundFunction::CreateTempLocalVariable(Cm::Sym::TypeSymbol* type)
{
    std::string tempVarName = "__temp" + std::to_string(int(temporaries.size()));
    Cm::Sym::LocalVariableSymbol* tempVar = new Cm::Sym::LocalVariableSymbol(Cm::Parsing::Span(), tempVarName);
    tempVar->SetType(type);
    temporaries.push_back(std::unique_ptr<Cm::Sym::LocalVariableSymbol>(tempVar));
    AddLocalVariable(tempVar);
    return tempVar;
}

void BoundFunction::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (visitor.VisitFunctionBody())
    {
        body->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundFunction::Own(Cm::Ast::Node* syntaxNode)
{
    syntaxNodes.push_back(std::unique_ptr<Cm::Ast::Node>(syntaxNode));
}

} } // namespace Cm::BoundTree