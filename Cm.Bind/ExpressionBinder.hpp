/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EXPRESSION_INCLUDED
#define CM_BIND_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/DerivedTypeOpRepository.hpp>
#include <Cm.Core/SynthesizedClassFunRepository.hpp>
#include <Cm.Core/StringRepository.hpp>
#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

void PrepareFunctionArguments(Cm::Sym::FunctionSymbol* fun, Cm::Sym::ContainerScope* containerScope, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::BoundTree::BoundFunction* currentFunction, 
    Cm::BoundTree::BoundExpressionList& arguments, bool firstArgByRef, Cm::Core::IrClassTypeRepository& irClassTypeRepository);

class BoundExpressionStack
{
public:
    void Push(Cm::BoundTree::BoundExpression* expression) { expressions.Add(expression); }
    Cm::BoundTree::BoundExpression* Pop();
    Cm::BoundTree::BoundExpressionList Pop(int numExpressions);
    Cm::BoundTree::BoundExpressionList GetExpressions() { return std::move(expressions); }
    int ItemCount() const { return expressions.Count(); }
private:
    Cm::BoundTree::BoundExpressionList expressions;
};

class ExpressionBinder : public Cm::Ast::Visitor
{
public:
    ExpressionBinder(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, Cm::Sym::ContainerScope* containerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_,
        Cm::BoundTree::BoundFunction* currentFunction_);
    Cm::BoundTree::BoundCompileUnit& BoundCompileUnit() { return boundCompileUnit; }
    Cm::Sym::ContainerScope* ContainerScope() const { return containerScope; }
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& FileScopes() const { return fileScopes; }
    Cm::BoundTree::BoundFunction* CurrentFunction() const { return currentFunction; }
    Cm::BoundTree::BoundExpressionList GetExpressions();
    Cm::BoundTree::BoundExpression* Pop() { return boundExpressionStack.Pop(); }
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
    void Visit(Cm::Ast::AddrOfNode& addrOfNode) override;
    void Visit(Cm::Ast::DerefNode& derefNode) override;
    void Visit(Cm::Ast::PostfixIncNode& postfixIncNode) override;
    void Visit(Cm::Ast::PostfixDecNode& postfixDecNode) override;
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
    void Visit(Cm::Ast::ArrowNode& arrowNode) override;
    void BeginVisit(Cm::Ast::InvokeNode& invokeNode) override;
    void EndVisit(Cm::Ast::InvokeNode& invokeNode) override;
    void Visit(Cm::Ast::IndexNode& indexNode) override;

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) override;
    void Visit(Cm::Ast::CastNode& castNode) override;
    void Visit(Cm::Ast::ConstructNode& constructNode) override;
    void Visit(Cm::Ast::NewNode& newNode) override;
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void Visit(Cm::Ast::ThisNode& thisNode) override;
    void Visit(Cm::Ast::BaseNode& baseNode) override;
    void Visit(Cm::Ast::TypeNameNode& typeNameNode) override;

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

    void Visit(Cm::Ast::BoolNode& boolNode) override;
    void Visit(Cm::Ast::SByteNode& sbyteNode) override;
    void Visit(Cm::Ast::ByteNode& byteNode) override;
    void Visit(Cm::Ast::ShortNode& shortNode) override;
    void Visit(Cm::Ast::UShortNode& shortNode) override;
    void Visit(Cm::Ast::IntNode& intNode) override;
    void Visit(Cm::Ast::UIntNode& uintNode) override;
    void Visit(Cm::Ast::LongNode& longNode) override;
    void Visit(Cm::Ast::ULongNode& ulongNode) override;
    void Visit(Cm::Ast::FloatNode& floatNode) override;
    void Visit(Cm::Ast::DoubleNode& doubleNode) override;
    void Visit(Cm::Ast::CharNode& charNode) override;
    void Visit(Cm::Ast::VoidNode& voidNode) override;
    void Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode) override;

    void GenerateTrueExpression(Cm::Ast::Node* node);
    void PrepareFunctionSymbol(Cm::Sym::FunctionSymbol* fun, const Cm::Parsing::Span& span);
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    Cm::Sym::ContainerScope* containerScope;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes;
    BoundExpressionStack boundExpressionStack;
    int expressionCount;
    std::stack<int> expressionCountStack;
    Cm::BoundTree::BoundFunction* currentFunction;
    void BindUnaryOp(Cm::Ast::Node* node, const std::string& opGroupName);
    void BindBinaryOp(Cm::Ast::Node* node, const std::string& opGroupName);
    void BindSymbol(Cm::Ast::Node* idNode, Cm::Sym::Symbol* symbol);
    void BindConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::ConstantSymbol* constantSymbol);
    void BindLocalVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::LocalVariableSymbol* localVariableSymbol);
    void BindMemberVariableSymbol(Cm::Ast::Node* idNode, Cm::Sym::MemberVariableSymbol* memberVariableSymbol);
    void BindParameterSymbol(Cm::Ast::Node* idNode, Cm::Sym::ParameterSymbol* parameterSymbol);
    void BindClassTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::ClassTypeSymbol* classTypeSymbol);
    void BindNamespaceSymbol(Cm::Ast::Node* idNode, Cm::Sym::NamespaceSymbol* namespaceSymbol);
    void BindEnumTypeSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumTypeSymbol* enumTypeSymbol);
    void BindEnumConstantSymbol(Cm::Ast::Node* idNode, Cm::Sym::EnumConstantSymbol* enumConstantSymbol);
    void BindTypedefSymbol(Cm::Ast::Node* idNode, Cm::Sym::TypedefSymbol* typedefSymbol);
    void BindBoundTypeParameterSymbol(Cm::Ast::Node* idNode, Cm::Sym::BoundTypeParameterSymbol* boundTypeParameterSymbol);
    void BindFunctionGroup(Cm::Ast::Node* idNode, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol);
    void BindIndexPointer(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index);
    void BindIndexClass(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index);
    void BindInvoke(Cm::Ast::Node* node, int numArgs);
    Cm::Sym::FunctionSymbol* BindInvokeConstructTemporary(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
        Cm::Sym::TypeSymbol* typeSymbol, Cm::Sym::LocalVariableSymbol*& temporary);
    Cm::Sym::FunctionSymbol* BindInvokeMemFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments, 
        bool& firstArgByRef, bool& generateVirtualCall, const std::string& functionGroupName, int& numArgs);
    Cm::Sym::FunctionSymbol* BindInvokeFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
        bool& firstArgByRef, bool& generateVirtualCall, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol, const std::vector<Cm::Sym::TypeSymbol*>& boundTemplateArguments);
    Cm::Sym::FunctionSymbol* BindInvokeOpApply(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
        Cm::Sym::TypeSymbol* plainSubjectType, Cm::BoundTree::BoundExpression* subject);
    void BindCast(Cm::Ast::Node* node, Cm::Ast::Node* targetTypeExpr, Cm::Ast::Node* sourceExpr, const Cm::Parsing::Span& span);
    void BindCast(Cm::Ast::Node* node, Cm::Sym::TypeSymbol* targetType, Cm::BoundTree::BoundExpression* sourceExpr);
    void BindConstruct(Cm::Ast::Node* node, Cm::Ast::Node* typeExpr, Cm::Ast::NodeList& argumentNodes);
    void BindConstruct(Cm::Ast::Node* node, Cm::Ast::Node* typeExpr, Cm::Ast::NodeList& argumentNodes, Cm::BoundTree::BoundExpression* allocationArg);
    void BindArrow(Cm::Ast::Node* node, const std::string& memberId);
};

Cm::BoundTree::TraceCallInfo* CreateTraceCallInfo(Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Sym::FunctionSymbol* fun, const Cm::Parsing::Span& span);

} } // namespace Cm::Bind

#endif // CM_BIND_EXPRESSION_INCLUDED
