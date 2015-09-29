/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <stdexcept>

namespace Cm { namespace BoundTree {

LandingPad::LandingPad(int id_, int jumpToCatchId_) : id(id_), jumpToCatchId(jumpToCatchId_)
{
}

void LandingPad::AddDestructionStatement(BoundDestructionStatement* destructionStatement)
{
    destructionStatements.push_back(std::unique_ptr<BoundDestructionStatement>(destructionStatement));
}

BoundFunction::BoundFunction() : BoundNode(nullptr), functionSymbol(), classObjectLayoutFunIndex(0), hasGotos(false), nextCatchId(0), currentTry(nullptr), inHandler(false), isRealMain(false), 
    isUserMain(false)
{
}

BoundFunction::BoundFunction(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_) :
    BoundNode(syntaxNode_), functionSymbol(functionSymbol_), classObjectLayoutFunIndex(0), hasGotos(false), nextCatchId(0), currentTry(nullptr), inHandler(false), isRealMain(false), isUserMain(false)
{
}

void BoundFunction::Write(Cm::Sym::BcuWriter& writer)
{
    BoundNode::Write(writer);
    writer.Write(functionSymbol);
    if (functionSymbol->Name() == "@static_constructor()")
    {
        int x = 0;
    }
    int n = int(localVariables.size());
    writer.GetBinaryWriter().Write(n);
    for (Cm::Sym::LocalVariableSymbol* localVariable : localVariables)
    {
        writer.GetSymbolWriter().Write(localVariable);
    }
    int nt = 0;
    for (const std::unique_ptr<Cm::Sym::LocalVariableSymbol>& temporary : temporaries)
    {
        if (temporary)
        {
            ++nt;
        }
    }
    writer.GetBinaryWriter().Write(nt);
    for (const std::unique_ptr<Cm::Sym::LocalVariableSymbol>& temporary : temporaries)
    {
        if (temporary)
        {
            writer.GetSymbolWriter().Write(temporary.get());
        }
    }
    writer.Write(body.get());
    writer.GetBinaryWriter().Write(isRealMain);
    isUserMain = functionSymbol == writer.GetSymbolWriter().GetSymbolTable()->UserMainFunction();
    writer.GetBinaryWriter().Write(isUserMain);
}

void BoundFunction::Read(Cm::Sym::BcuReader& reader)
{
    BoundNode::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsFunctionSymbol())
    {
        functionSymbol = static_cast<Cm::Sym::FunctionSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    if (functionSymbol->Name() == "@static_constructor()")
    {
        int x = 0;
    }
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::Symbol* l = reader.GetSymbolReader().ReadSymbol();
        if (l->IsLocalVariableSymbol())
        {
            localVariables.push_back(static_cast<Cm::Sym::LocalVariableSymbol*>(l));
            reader.GetSymbolReader().GetSymbolTable().Own(static_cast<Cm::Sym::LocalVariableSymbol*>(l));
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
    int nt = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < nt; ++i)
    {
        Cm::Sym::Symbol* t = reader.GetSymbolReader().ReadSymbol();
        if (t->IsLocalVariableSymbol())
        {
            reader.GetSymbolReader().GetSymbolTable().Own(static_cast<Cm::Sym::LocalVariableSymbol*>(t));
        }
        else
        {
            throw std::runtime_error("local variable symbol expected");
        }
    }
    Cm::Sym::BcuItem* bodyItem = reader.ReadItem();
    if (bodyItem)
    {
        if (bodyItem->IsBoundCompoundStatement())
        {
            body.reset(static_cast<BoundCompoundStatement*>(bodyItem));
        }
        else
        {
            throw std::runtime_error("bound compound statement expected");
        }
    }
    isRealMain = reader.GetBinaryReader().ReadBool();
    isUserMain = reader.GetBinaryReader().ReadBool();
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

void BoundFunction::AddLandingPad(LandingPad* landingPad)
{
    landingPads.push_back(std::unique_ptr<LandingPad>(landingPad));
}

void BoundFunction::Own(Cm::Ast::Node* syntaxNode)
{
    syntaxNodes.push_back(std::unique_ptr<Cm::Ast::Node>(syntaxNode));
}

void BoundFunction::PushTryNode(Cm::Ast::TryStatementNode* tryNode)
{
    tryNodeStack.push_back(currentTry);
    currentTry = tryNode;
}

void BoundFunction::PopTryNode()
{
    currentTry = tryNodeStack.back();
    tryNodeStack.pop_back();
}

Cm::Ast::TryStatementNode* BoundFunction::GetParentTry()
{
    Cm::Ast::TryStatementNode* parentTry = nullptr;
    if (!tryNodeStack.empty())
    {
        parentTry = tryNodeStack.back();
    }
    if (parentTry)
    {
        return parentTry;
    }
    return currentTry;
}

void BoundFunction::PushHandler()
{
    inHandlerStack.push(inHandler);
    inHandler = true;
}

void BoundFunction::PopHandler()
{
    inHandler = inHandlerStack.top();
    inHandlerStack.pop();
}

void BoundFunction::AddTryCompound(Cm::Ast::TryStatementNode* tryNode, BoundCompoundStatement* tryCompound)
{
    tryCompoundMap[tryNode] = tryCompound;
}

BoundCompoundStatement* BoundFunction::GetTryCompound(Cm::Ast::TryStatementNode* tryNode)
{
    TryCompoundMapIt i = tryCompoundMap.find(tryNode);
    if (i != tryCompoundMap.end())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("try compound not found");
    }
}

} } // namespace Cm::BoundTree
