/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EXPRESSION_INCLUDED
#define CM_BIND_EXPRESSION_INCLUDED
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Core/ClassConversionTable.hpp>
#include <Cm.Core/DerivedTypeOpRepository.hpp>
#include <Cm.Core/SynthesizedClassFunRepository.hpp>
#include <Cm.Core/StringRepository.hpp>
#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

void PrepareFunctionArguments(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::BoundExpressionList& arguments, bool firstArgByRef, Cm::Core::IrClassTypeRepository& irClassTypeRepository);

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
    ExpressionBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ConversionTable& conversionTable_, Cm::Core::ClassConversionTable& classConversionTable_, 
        Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository_, Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository,
        Cm::Core::StringRepository& stringRepository_, Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Sym::ContainerScope* containerScope_, Cm::Sym::FileScope* fileScope_, 
        Cm::BoundTree::BoundFunction* currentFunction_);
    Cm::Sym::SymbolTable& SymbolTable() { return symbolTable; }
    Cm::Sym::ConversionTable& ConversionTable() { return conversionTable; }
    Cm::Core::ClassConversionTable& ClassConversionTable() { return classConversionTable; }
    Cm::Core::DerivedTypeOpRepository& DerivedTypeOpRepository() { return derivedTypeOpRepository; }
    Cm::Core::SynthesizedClassFunRepository& SynthesizedClassFunRepository() { return synthesizedClassFunRepository; }
    Cm::Core::StringRepository& StringRepository() { return stringRepository; }
    Cm::Core::IrClassTypeRepository& IrClassTypeRepository() { return irClassTypeRepository; }
    Cm::Sym::ContainerScope* ContainerScope() const { return containerScope; }
    Cm::Sym::FileScope* FileScope() const { return fileScope; }
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

    void Visit(Cm::Ast::SizeOfNode& sizeOfNode) {}
    void Visit(Cm::Ast::CastNode& castNode) override;
    void BeginVisit(Cm::Ast::ConstructNode& constructNode) {}
    void EndVisit(Cm::Ast::ConstructNode& constructNode) {}
    void BeginVisit(Cm::Ast::NewNode& newNode) {}
    void EndVisit(Cm::Ast::NewNode& newNode) {}
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) {}
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void Visit(Cm::Ast::ThisNode& thisNode) override;
    void Visit(Cm::Ast::BaseNode& baseNode) override;
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

    void GenerateTrueExpression(Cm::Ast::Node* node);
    void PrepareFunctionSymbol(Cm::Sym::FunctionSymbol* fun, const Cm::Parsing::Span& span);
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ConversionTable& conversionTable;
    Cm::Core::ClassConversionTable& classConversionTable;
    Cm::Core::DerivedTypeOpRepository& derivedTypeOpRepository;
    Cm::Core::SynthesizedClassFunRepository& synthesizedClassFunRepository;
    Cm::Core::StringRepository& stringRepository;
    Cm::Core::IrClassTypeRepository& irClassTypeRepository;
    Cm::Sym::ContainerScope* containerScope;
    Cm::Sym::FileScope* fileScope;
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
    void BindFunctionGroup(Cm::Ast::Node* idNode, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol);
    void BindIndexPointer(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index);
    void BindIndexClass(Cm::Ast::Node* indexNode, Cm::BoundTree::BoundExpression* subject, Cm::BoundTree::BoundExpression* index);
    void BindInvoke(Cm::Ast::Node* node, int numArgs);
    Cm::Sym::FunctionSymbol* BindInvokeMemFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments, 
        bool& firstArgByRef, bool& generateVirtualCall, const std::string& functionGroupName, int& numArgs);
    Cm::Sym::FunctionSymbol* BindInvokeFun(Cm::Ast::Node* node, std::vector<Cm::Sym::FunctionSymbol*>& conversions, Cm::BoundTree::BoundExpressionList& arguments,
        bool& firstArgByRef, bool& generateVirtualCall, Cm::Sym::FunctionGroupSymbol* functionGroupSymbol);
    void BindArrow(Cm::Ast::Node* node, const std::string& memberId);
};

} } // namespace Cm::Bind

#endif // CM_BIND_EXPRESSION_INCLUDED
