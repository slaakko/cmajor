/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EXPRESSION_INCLUDED
#define CM_BIND_EXPRESSION_INCLUDED
#include <Cm.BoundTree/Expression.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

class BoundNodeStack
{
public:
    void Push(Cm::BoundTree::BoundNode* boundNode) { boundNodeStack.push_back(boundNode); }
    Cm::BoundTree::BoundNode* Pop();
    Cm::BoundTree::BoundExpression* PopExpr();
    Cm::BoundTree::BoundExpressionList GetExpressions();
private:
    std::vector<Cm::BoundTree::BoundNode*> boundNodeStack;
};

class ExpressionBinder : public Cm::Ast::Visitor
{
public:
    ExpressionBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_);
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ContainerScope* ContainerScope() const { return containerScope; }
    Cm::Sym::FileScope* FileScope() const { return fileScope; }
    Cm::BoundTree::BoundExpressionList GetArguments();
    void BeginVisit(Cm::Ast::DisjunctionNode& disjunctionNode) {}
    void EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode) {}
    void BeginVisit(Cm::Ast::ConjunctionNode& conjunctionNode) {}
    void EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode) {}
    void BeginVisit(Cm::Ast::BitOrNode& bitOrNode) {}
    void EndVisit(Cm::Ast::BitOrNode& bitOrNode) {}
    void BeginVisit(Cm::Ast::BitXorNode& bitXorNode) {}
    void EndVisit(Cm::Ast::BitXorNode& bitXorNode) {}
    void BeginVisit(Cm::Ast::BitAndNode& bitAndNode) {}
    void EndVisit(Cm::Ast::BitAndNode& bitAndNode) {}
    void BeginVisit(Cm::Ast::EqualNode& equalNode) {}
    void EndVisit(Cm::Ast::EqualNode& equalNode) {}
    void BeginVisit(Cm::Ast::NotEqualNode& notEqualNode) {}
    void EndVisit(Cm::Ast::NotEqualNode& notEqualNode) {}
    void BeginVisit(Cm::Ast::LessNode& lessNode) {}
    void EndVisit(Cm::Ast::LessNode& lessNode) {}
    void BeginVisit(Cm::Ast::GreaterNode& greaterNode) {}
    void EndVisit(Cm::Ast::GreaterNode& greaterNode) {}
    void BeginVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode) {}
    void EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode) {}
    void BeginVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode) {}
    void EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode) {}
    void BeginVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode) {}
    void EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode) {}
    void BeginVisit(Cm::Ast::ShiftRightNode& shiftRightNode) {}
    void EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode) {}
    void EndVisit(Cm::Ast::AddNode& addNode) override;
    void BeginVisit(Cm::Ast::SubNode& subNode) {}
    void EndVisit(Cm::Ast::SubNode& subNode) {}
    void BeginVisit(Cm::Ast::MulNode& mulNode) {}
    void EndVisit(Cm::Ast::MulNode& mulNode) {}
    void BeginVisit(Cm::Ast::DivNode& divNode) {}
    void EndVisit(Cm::Ast::DivNode& divNode) {}
    void BeginVisit(Cm::Ast::RemNode& remNode) {}
    void EndVisit(Cm::Ast::RemNode& remNode) {}
    void BeginVisit(Cm::Ast::PrefixIncNode& prefixIncNode) {}
    void EndVisit(Cm::Ast::PrefixIncNode& prefixIncNode) {}
    void BeginVisit(Cm::Ast::PrefixDecNode& prefixDecNode) {}
    void EndVisit(Cm::Ast::PrefixDecNode& prefixDecNode) {}
    void BeginVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) {}
    void EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) {}
    void BeginVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode) {}
    void EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode) {}
    void BeginVisit(Cm::Ast::NotNode& notNode) {}
    void EndVisit(Cm::Ast::NotNode& notNode) {}
    void BeginVisit(Cm::Ast::ComplementNode& complementNode) {}
    void EndVisit(Cm::Ast::ComplementNode& complementNode) {}
    void Visit(Cm::Ast::AddrOfNode& addrOfNode) {}
    void Visit(Cm::Ast::DerefNode& derefNode) {}
    void Visit(Cm::Ast::PostfixIncNode& postfixIncNode) {}
    void Visit(Cm::Ast::PostfixDecNode& postfixDecNode) {}
    void EndVisit(Cm::Ast::PostfixDecNode& postfixDecNode) {}
    void Visit(Cm::Ast::DotNode& dotNode) {}
    void Visit(Cm::Ast::ArrowNode& arrowNode) {}
    void BeginVisit(Cm::Ast::InvokeNode& invokeNode) {}
    void EndVisit(Cm::Ast::InvokeNode& invokeNode) {}
    void Visit(Cm::Ast::IndexNode& indexNode) {}

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) {}
    void Visit(Cm::Ast::CastNode& castNode) {}
    void BeginVisit(Cm::Ast::ConstructNode& constructNode) {}
    void EndVisit(Cm::Ast::ConstructNode& constructNode) {}
    void BeginVisit(Cm::Ast::NewNode& newNode) {}
    void EndVisit(Cm::Ast::NewNode& newNode) {}
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) {}
    void Visit(Cm::Ast::IdentifierNode& identifierNode) {}
    void Visit(Cm::Ast::ThisNode& thisNode) {}
    void Visit(Cm::Ast::BaseNode& baseNode) {}
    void Visit(Cm::Ast::TypeNameNode& typeNameNode) {}

    void Visit(Cm::Ast::BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(Cm::Ast::SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(Cm::Ast::ByteLiteralNode& byteLiteralNode) override;
    void Visit(Cm::Ast::ShortLiteralNode& shortLiteralNode) override;
    void Visit(Cm::Ast::UShortLiteralNode& ushortLiteralNode) override;
    void Visit(Cm::Ast::IntLiteralNode& intLiteralNode) override;
    void Visit(Cm::Ast::UIntLiteralNode& uintLiteralNode) override;
    void Visit(Cm::Ast::LongLiteralNode& longLiteralNode) override;
    void Visit(Cm::Ast::ULongLiteralNode& ulongLiteralNode) override;
    void Visit(Cm::Ast::FloatLiteralNode& floatLiteralNode) override;
    void Visit(Cm::Ast::DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(Cm::Ast::CharLiteralNode& charLiteralNode) override;
    void Visit(Cm::Ast::StringLiteralNode& stringLiteralNode) override;
    void Visit(Cm::Ast::NullLiteralNode& nullLiteralNode) override;

    void Visit(Cm::Ast::BoolNode& boolNode) {}
    void Visit(Cm::Ast::SByteNode& sbyteNode) {}
    void Visit(Cm::Ast::ByteNode& byteNode) {}
    void Visit(Cm::Ast::ShortNode& shortNode) {}
    void Visit(Cm::Ast::UShortNode& shortNode) {}
    void Visit(Cm::Ast::IntNode& intNode) {}
    void Visit(Cm::Ast::UIntNode& uintNode) {}
    void Visit(Cm::Ast::LongNode& longNode) {}
    void Visit(Cm::Ast::ULongNode& ulongNode) {}
    void Visit(Cm::Ast::FloatNode& floatNode) {}
    void Visit(Cm::Ast::DoubleNode& doubleNode) {}
    void Visit(Cm::Ast::CharNode& charNode) {}
    void Visit(Cm::Ast::VoidNode& voidNode) {}
    void Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode) {}
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* containerScope;
    Cm::Sym::FileScope* fileScope;
    BoundNodeStack boundNodeStack;
};

} } // namespace Cm::Bind

#endif // CM_BIND_EXPRESSION_INCLUDED
