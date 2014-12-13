
/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Expression.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Literal.hpp>

namespace Cm { namespace Bind {

using Cm::Parsing::Span;

Cm::BoundTree::BoundNode* BoundNodeStack::Pop()
{
    if (boundNodeStack.empty()) throw std::runtime_error("bound node stack is empty");
    Cm::BoundTree::BoundNode* top = boundNodeStack.back();
    boundNodeStack.pop_back();
    return top;
}

Cm::BoundTree::BoundExpression* BoundNodeStack::PopExpr()
{
    Cm::BoundTree::BoundNode* top = Pop();
    if (top->IsBoundExpressionNode())
    {
        return static_cast<Cm::BoundTree::BoundExpression*>(top);
    }
    else
    {
        if (top->SyntaxNode())
        {
            Span span = top->SyntaxNode()->GetSpan();
            throw Exception("expression expected", span);
        }
        else
        {
            throw Exception("expression expected");
        }
    }
}

Cm::BoundTree::BoundExpressionList BoundNodeStack::GetExpressions()
{
    Cm::BoundTree::BoundExpressionList expressions;
    for (Cm::BoundTree::BoundNode* boundNode : boundNodeStack)
    {
        if (boundNode->IsBoundExpressionNode())
        {
            expressions.AddExpression(static_cast<Cm::BoundTree::BoundExpression*>(boundNode));
        }
        else
        {
            if (boundNode->SyntaxNode())
            {
                Span span = boundNode->SyntaxNode()->GetSpan();
                throw Exception("expression expected", span);
            }
            else
            {
                throw Exception("expression expected");
            }
        }
    }
    return expressions;
}

ExpressionBinder::ExpressionBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_) : 
    Cm::Ast::Visitor(true), symbolTable(symbolTable_), containerScope(containerScope_), fileScope(fileScope_)
{
}

Cm::BoundTree::BoundExpressionList ExpressionBinder::GetArguments()
{
    return boundNodeStack.GetExpressions();
}

void ExpressionBinder::EndVisit(Cm::Ast::AddNode& addNode)
{
    Cm::BoundTree::BoundExpression* right = boundNodeStack.PopExpr();
    Cm::BoundTree::BoundExpression* left = boundNodeStack.PopExpr();
    boundNodeStack.Push(new Cm::BoundTree::BoundBinaryOp(&addNode, left, right));
}

void ExpressionBinder::Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
    Cm::Sym::Value* value = new Cm::Sym::BoolValue(booleanLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&booleanLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
    Cm::Sym::Value* value = new Cm::Sym::SByteValue(sbyteLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&sbyteLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
    Cm::Sym::Value* value = new Cm::Sym::ByteValue(byteLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&byteLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
    Cm::Sym::Value* value = new Cm::Sym::ShortValue(shortLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&shortLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
    Cm::Sym::Value* value = new Cm::Sym::UShortValue(ushortLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ushortLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::IntLiteralNode& intLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
    Cm::Sym::Value* value = new Cm::Sym::IntValue(intLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&intLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
    Cm::Sym::Value* value = new Cm::Sym::UIntValue(uintLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&uintLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::LongLiteralNode& longLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
    Cm::Sym::Value* value = new Cm::Sym::LongValue(longLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&longLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
    Cm::Sym::Value* value = new Cm::Sym::ULongValue(ulongLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&ulongLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
    Cm::Sym::Value* value = new Cm::Sym::FloatValue(floatLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&floatLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
    Cm::Sym::Value* value = new Cm::Sym::DoubleValue(doubleLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&doubleLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::CharLiteralNode& charLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
    Cm::Sym::Value* value = new Cm::Sym::CharValue(charLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&charLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::StringLiteralNode& stringLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().MakeConstCharPtrType(stringLiteralNode.GetSpan());
    Cm::Sym::Value* value = new Cm::Sym::StringValue(stringLiteralNode.Value());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&stringLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(value);
    boundNodeStack.Push(literalNode);
}

void ExpressionBinder::Visit(Cm::Ast::NullLiteralNode& nullLiteralNode)
{
    Cm::Sym::TypeSymbol* type = symbolTable.GetTypeRepository().MakeGenericPtrType(nullLiteralNode.GetSpan());
    Cm::BoundTree::BoundLiteral* literalNode = new Cm::BoundTree::BoundLiteral(&nullLiteralNode);
    literalNode->SetType(type);
    literalNode->SetValue(nullptr);
    boundNodeStack.Push(literalNode);
}

} } // namespace Cm::Bind