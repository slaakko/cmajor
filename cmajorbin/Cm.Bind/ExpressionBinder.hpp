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
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

class BoundExpressionStack
{
public:
    void Push(Cm::BoundTree::BoundExpression* expression) { expressions.Add(expression); }
    Cm::BoundTree::BoundExpression* Pop();
    Cm::BoundTree::BoundExpressionList GetExpressions() { return std::move(expressions); }
private:
    Cm::BoundTree::BoundExpressionList expressions;
};

class ExpressionBinder : public Cm::Ast::Visitor
{
public:
    ExpressionBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
        Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, Cm::Sym::FunctionSymbol* currentFunction_);
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ConversionTable& ConversionTable() { return conversionTable; }
    Cm::Core::ClassConversionTable& ClassConversionTable() { return classConversionTable; }
    Cm::Sym::ContainerScope* ContainerScope() const { return containerScope; }
    Cm::Sym::FileScope* FileScope() const { return fileScope; }
    Cm::BoundTree::BoundExpressionList GetExpressions();
    void EndVisit(Cm::Ast::DisjunctionNode& disjunctionNode) override;
    void EndVisit(Cm::Ast::ConjunctionNode& conjunctionNode) override;
    void EndVisit(Cm::Ast::BitOrNode& bitOrNode) override;
    void EndVisit(Cm::Ast::BitXorNode& bitXorNode) override;
    void EndVisit(Cm::Ast::BitAndNode& bitAndNode) override;
    void EndVisit(Cm::Ast::EqualNode& equalNode) override;
    void EndVisit(Cm::Ast::NotEqualNode& notEqualNode) override;
    void EndVisit(Cm::Ast::LessNode& lessNode) override;
    void EndVisit(Cm::Ast::GreaterNode& greaterNode) override;
    void EndVisit(Cm::Ast::LessOrEqualNode& lessOrEqualNode) override;
    void EndVisit(Cm::Ast::GreaterOrEqualNode& greaterOrEqualNode) override;
    void EndVisit(Cm::Ast::ShiftLeftNode& shiftLeftNode) override;
    void EndVisit(Cm::Ast::ShiftRightNode& shiftRightNode) override;
    void EndVisit(Cm::Ast::AddNode& addNode) override;
    void EndVisit(Cm::Ast::SubNode& subNode) override;
    void EndVisit(Cm::Ast::MulNode& mulNode) override;
    void EndVisit(Cm::Ast::DivNode& divNode) override;
    void EndVisit(Cm::Ast::RemNode& remNode) override;
    void EndVisit(Cm::Ast::PrefixIncNode& prefixIncNode) override;
    void EndVisit(Cm::Ast::PrefixDecNode& prefixDecNode) override;
    void EndVisit(Cm::Ast::UnaryPlusNode& unaryPlusNode) override;
    void EndVisit(Cm::Ast::UnaryMinusNode& unaryMinusNode) override;
    void EndVisit(Cm::Ast::NotNode& notNode) override;
    void EndVisit(Cm::Ast::ComplementNode& complementNode) override;
    void Visit(Cm::Ast::AddrOfNode& addrOfNode) {}
    void Visit(Cm::Ast::DerefNode& derefNode) {}
    void Visit(Cm::Ast::PostfixIncNode& postfixIncNode) {}
    void Visit(Cm::Ast::PostfixDecNode& postfixDecNode) {}
    void EndVisit(Cm::Ast::PostfixDecNode& postfixDecNode) {}
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
    void Visit(Cm::Ast::ArrowNode& arrowNode) {}
    void BeginVisit(Cm::Ast::InvokeNode& invokeNode) {}
    void EndVisit(Cm::Ast::InvokeNode& invokeNode) {}
    void Visit(Cm::Ast::IndexNode& indexNode) {}

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) {}
    void Visit(Cm::Ast::CastNode& castNode) override;
    void BeginVisit(Cm::Ast::ConstructNode& constructNode) {}
    void EndVisit(Cm::Ast::ConstructNode& constructNode) {}
    void BeginVisit(Cm::Ast::NewNode& newNode) {}
    void EndVisit(Cm::Ast::NewNode& newNode) {}
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) {}
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
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
    Cm::Sym::ConversionTable& conversionTable;
    Cm::Core::ClassConversionTable& classConversionTable;
    Cm::Sym::ContainerScope* containerScope;
    Cm::Sym::FileScope* fileScope;
    BoundExpressionStack boundExpressionStack;
    Cm::Sym::FunctionSymbol* currentFunction;
    void BindUnaryOp(Cm::Ast::Node* node, const std::string& opGroupName);
    void BindBinaryOp(Cm::Ast::Node* node, const std::string& opGroupName);
    void BindSymbol(Cm::Ast::Node* idNode, Cm::Sym::Symbol* symbol);
    void BindConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::ConstantSymbol* constantSymbol);
    void BindLocalVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::LocalVariableSymbol* localVariableSymbol);
    void BindMemberVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::MemberVariableSymbol* memberVariableSymbol);
    void BindClassTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::ClassTypeSymbol* classTypeSymbol);
    void BindNamespaceSymbol(Cm::Ast::Node* idNode, Cm::Sym::NamespaceSymbol* namespaceSymbol);
    void BindEnumTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumTypeSymbol* enumTypeSymbol);
};

} } // namespace Cm::Bind

#endif // CM_BIND_EXPRESSION_INCLUDED
