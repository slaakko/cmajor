/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/Factory.hpp>
#include <Cm.Ast/BasicType.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Template.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Constant.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Delegate.hpp>
#include <Cm.Ast/Typedef.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Interface.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/CompileUnit.hpp>

namespace Cm { namespace Ast {

NodeCreator::~NodeCreator()
{
}

template<typename T>
class Creator: public NodeCreator
{
public:
    virtual Node* CreateNode(Span span) 
    {
        return new T(span);
    }
};

std::unique_ptr<Factory> Factory::instance;

void Factory::Init()
{
    instance = std::unique_ptr<Factory>(new Factory());
}

Factory& Factory::Instance()
{
    return *instance;
}

void Factory::Done()
{
    instance.reset();
}

Factory::Factory()
{
    creators.resize(int(NodeType::maxNode));
}

void Factory::Register(NodeType nodeType, NodeCreator* creator)
{
    creators[int(nodeType)] = std::unique_ptr<NodeCreator>(creator);
}

Node* Factory::CreateNode(NodeType nodeType, Span span)
{
    if (!creators[int(nodeType)])
    {
        throw std::runtime_error("no creator for node type " + std::to_string(int(nodeType)));
    }
    Node* node = creators[int(nodeType)]->CreateNode(span);
    return node;
}

void InitFactory()
{
    Factory::Init();
    Factory::Instance().Register(NodeType::boolNode, new Creator<BoolNode>());
    Factory::Instance().Register(NodeType::sbyteNode, new Creator<SByteNode>());
    Factory::Instance().Register(NodeType::byteNode, new Creator<ByteNode>());
    Factory::Instance().Register(NodeType::shortNode, new Creator<ShortNode>());
    Factory::Instance().Register(NodeType::ushortNode, new Creator<UShortNode>());
    Factory::Instance().Register(NodeType::intNode, new Creator<IntNode>());
    Factory::Instance().Register(NodeType::uintNode, new Creator<UIntNode>());
    Factory::Instance().Register(NodeType::longNode, new Creator<LongNode>());
    Factory::Instance().Register(NodeType::ulongNode, new Creator<ULongNode>());
    Factory::Instance().Register(NodeType::floatNode, new Creator<FloatNode>());
    Factory::Instance().Register(NodeType::doubleNode, new Creator<DoubleNode>());
    Factory::Instance().Register(NodeType::charNode, new Creator<CharNode>());
    Factory::Instance().Register(NodeType::wcharNode, new Creator<WCharNode>());
    Factory::Instance().Register(NodeType::ucharNode, new Creator<UCharNode>());
    Factory::Instance().Register(NodeType::voidNode, new Creator<VoidNode>());
    Factory::Instance().Register(NodeType::booleanLiteralNode, new Creator<BooleanLiteralNode>());
    Factory::Instance().Register(NodeType::sbyteLiteralNode, new Creator<SByteLiteralNode>());
    Factory::Instance().Register(NodeType::byteLiteralNode, new Creator<ByteLiteralNode>());
    Factory::Instance().Register(NodeType::shortLiteralNode, new Creator<ShortLiteralNode>());
    Factory::Instance().Register(NodeType::ushortLiteralNode, new Creator<UShortLiteralNode>());
    Factory::Instance().Register(NodeType::intLiteralNode, new Creator<IntLiteralNode>());
    Factory::Instance().Register(NodeType::uintLiteralNode, new Creator<UIntLiteralNode>());
    Factory::Instance().Register(NodeType::longLiteralNode, new Creator<LongLiteralNode>());
    Factory::Instance().Register(NodeType::ulongLiteralNode, new Creator<ULongLiteralNode>());
    Factory::Instance().Register(NodeType::floatLiteralNode, new Creator<FloatLiteralNode>());
    Factory::Instance().Register(NodeType::doubleLiteralNode, new Creator<DoubleLiteralNode>());
    Factory::Instance().Register(NodeType::charLiteralNode, new Creator<CharLiteralNode>());
    Factory::Instance().Register(NodeType::stringLiteralNode, new Creator<StringLiteralNode>());
    Factory::Instance().Register(NodeType::wstringLiteralNode, new Creator<WStringLiteralNode>());
    Factory::Instance().Register(NodeType::ustringLiteralNode, new Creator<UStringLiteralNode>());
    Factory::Instance().Register(NodeType::nullLiteralNode, new Creator<NullLiteralNode>());
    Factory::Instance().Register(NodeType::derivedTypeExprNode, new Creator<DerivedTypeExprNode>());
    Factory::Instance().Register(NodeType::equivalenceNode, new Creator<EquivalenceNode>());
    Factory::Instance().Register(NodeType::implicationNode, new Creator<ImplicationNode>());
    Factory::Instance().Register(NodeType::disjunctionNode, new Creator<DisjunctionNode>());
    Factory::Instance().Register(NodeType::conjunctionNode, new Creator<ConjunctionNode>());
    Factory::Instance().Register(NodeType::bitOrNode, new Creator<BitOrNode>());
    Factory::Instance().Register(NodeType::bitXorNode, new Creator<BitXorNode>());
    Factory::Instance().Register(NodeType::bitAndNode, new Creator<BitAndNode>());
    Factory::Instance().Register(NodeType::equalNode, new Creator<EqualNode>());
    Factory::Instance().Register(NodeType::notEqualNode, new Creator<NotEqualNode>());
    Factory::Instance().Register(NodeType::lessNode, new Creator<LessNode>());
    Factory::Instance().Register(NodeType::greaterNode, new Creator<GreaterNode>());
    Factory::Instance().Register(NodeType::lessOrEqualNode, new Creator<LessOrEqualNode>());
    Factory::Instance().Register(NodeType::greaterOrEqualNode, new Creator<GreaterOrEqualNode>());
    Factory::Instance().Register(NodeType::shiftLeftNode, new Creator<ShiftLeftNode>());
    Factory::Instance().Register(NodeType::shiftRightNode, new Creator<ShiftRightNode>());
    Factory::Instance().Register(NodeType::addNode, new Creator<AddNode>());
    Factory::Instance().Register(NodeType::subNode, new Creator<SubNode>());
    Factory::Instance().Register(NodeType::mulNode, new Creator<MulNode>());
    Factory::Instance().Register(NodeType::divNode, new Creator<DivNode>());
    Factory::Instance().Register(NodeType::remNode, new Creator<RemNode>());
    Factory::Instance().Register(NodeType::invokeNode, new Creator<InvokeNode>());
    Factory::Instance().Register(NodeType::indexNode, new Creator<IndexNode>());
    Factory::Instance().Register(NodeType::dotNode, new Creator<DotNode>());
    Factory::Instance().Register(NodeType::arrowNode, new Creator<ArrowNode>());
    Factory::Instance().Register(NodeType::postfixIncNode, new Creator<PostfixIncNode>());
    Factory::Instance().Register(NodeType::postfixDecNode, new Creator<PostfixDecNode>());
    Factory::Instance().Register(NodeType::derefNode, new Creator<DerefNode>());
    Factory::Instance().Register(NodeType::addrOfNode, new Creator<AddrOfNode>());
    Factory::Instance().Register(NodeType::notNode, new Creator<NotNode>());
    Factory::Instance().Register(NodeType::unaryPlusNode, new Creator<UnaryPlusNode>());
    Factory::Instance().Register(NodeType::unaryMinusNode, new Creator<UnaryMinusNode>());
    Factory::Instance().Register(NodeType::complementNode, new Creator<ComplementNode>());
    Factory::Instance().Register(NodeType::prefixIncNode, new Creator<PrefixIncNode>());
    Factory::Instance().Register(NodeType::prefixDecNode, new Creator<PrefixDecNode>());
    Factory::Instance().Register(NodeType::sizeOfNode, new Creator<SizeOfNode>());
    Factory::Instance().Register(NodeType::typeNameNode, new Creator<TypeNameNode>());
    Factory::Instance().Register(NodeType::castNode, new Creator<CastNode>());
    Factory::Instance().Register(NodeType::isNode, new Creator<IsNode>());
    Factory::Instance().Register(NodeType::asNode, new Creator<AsNode>());
    Factory::Instance().Register(NodeType::newNode, new Creator<NewNode>());
    Factory::Instance().Register(NodeType::constructNode, new Creator<ConstructNode>());
    Factory::Instance().Register(NodeType::thisNode, new Creator<ThisNode>());
    Factory::Instance().Register(NodeType::baseNode, new Creator<BaseNode>());
    Factory::Instance().Register(NodeType::identifierNode, new Creator<IdentifierNode>());
    Factory::Instance().Register(NodeType::templateIdNode, new Creator<TemplateIdNode>());
    Factory::Instance().Register(NodeType::enumTypeNode, new Creator<EnumTypeNode>());
    Factory::Instance().Register(NodeType::enumConstantNode, new Creator<EnumConstantNode>());
    Factory::Instance().Register(NodeType::constantNode, new Creator<ConstantNode>());
    Factory::Instance().Register(NodeType::parameterNode, new Creator<ParameterNode>());
    Factory::Instance().Register(NodeType::delegateNode, new Creator<DelegateNode>());
    Factory::Instance().Register(NodeType::classDelegateNode, new Creator<ClassDelegateNode>());
    Factory::Instance().Register(NodeType::typedefNode, new Creator<TypedefNode>());
    Factory::Instance().Register(NodeType::labelNode, new Creator<LabelNode>());
    Factory::Instance().Register(NodeType::simpleStatementNode, new Creator<SimpleStatementNode>());
    Factory::Instance().Register(NodeType::returnStatementNode, new Creator<ReturnStatementNode>());
    Factory::Instance().Register(NodeType::conditionalStatementNode, new Creator<ConditionalStatementNode>());
    Factory::Instance().Register(NodeType::switchStatementNode, new Creator<SwitchStatementNode>());
    Factory::Instance().Register(NodeType::caseStatementNode, new Creator<CaseStatementNode>());
    Factory::Instance().Register(NodeType::defaultStatementNode, new Creator<DefaultStatementNode>());
    Factory::Instance().Register(NodeType::gotoCaseStatementNode, new Creator<GotoCaseStatementNode>());
    Factory::Instance().Register(NodeType::gotoDefaultStatementNode, new Creator<GotoDefaultStatementNode>());
    Factory::Instance().Register(NodeType::whileStatementNode, new Creator<WhileStatementNode>());
    Factory::Instance().Register(NodeType::doStatementNode, new Creator<DoStatementNode>());
    Factory::Instance().Register(NodeType::rangeForStatementNode, new Creator<RangeForStatementNode>());
    Factory::Instance().Register(NodeType::forStatementNode, new Creator<ForStatementNode>());
    Factory::Instance().Register(NodeType::compoundStatementNode, new Creator<CompoundStatementNode>());
    Factory::Instance().Register(NodeType::breakStatementNode, new Creator<BreakStatementNode>());
    Factory::Instance().Register(NodeType::continueStatementNode, new Creator<ContinueStatementNode>());
    Factory::Instance().Register(NodeType::gotoStatementNode, new Creator<GotoStatementNode>());
    Factory::Instance().Register(NodeType::typedefStatementNode, new Creator<TypedefStatementNode>());
    Factory::Instance().Register(NodeType::assignmentStatementNode, new Creator<AssignmentStatementNode>());
    Factory::Instance().Register(NodeType::constructionStatementNode, new Creator<ConstructionStatementNode>());
    Factory::Instance().Register(NodeType::deleteStatementNode, new Creator<DeleteStatementNode>());
    Factory::Instance().Register(NodeType::destroyStatementNode, new Creator<DestroyStatementNode>());
    Factory::Instance().Register(NodeType::throwStatementNode, new Creator<ThrowStatementNode>());
    Factory::Instance().Register(NodeType::tryStatementNode, new Creator<TryStatementNode>());
    Factory::Instance().Register(NodeType::catchNode, new Creator<CatchNode>());
    Factory::Instance().Register(NodeType::assertStatementNode, new Creator<AssertStatementNode>());
    Factory::Instance().Register(NodeType::condCompDisjunctionNode, new Creator<CondCompDisjunctionNode>());
    Factory::Instance().Register(NodeType::condCompConjunctionNode, new Creator<CondCompConjunctionNode>());
    Factory::Instance().Register(NodeType::condCompNotNode, new Creator<CondCompNotNode>());
    Factory::Instance().Register(NodeType::condCompPrimaryNode, new Creator<CondCompPrimaryNode>());
    Factory::Instance().Register(NodeType::condCompSymbolNode, new Creator<CondCompSymbolNode>());
    Factory::Instance().Register(NodeType::condCompilationPartNode, new Creator<CondCompilationPartNode>());
    Factory::Instance().Register(NodeType::condCompStatementNode, new Creator<CondCompStatementNode>());
    Factory::Instance().Register(NodeType::disjunctiveConstraintNode, new Creator<DisjunctiveConstraintNode>());
    Factory::Instance().Register(NodeType::conjunctiveConstraintNode, new Creator<ConjunctiveConstraintNode>());
    Factory::Instance().Register(NodeType::whereConstraintNode, new Creator<WhereConstraintNode>());
    Factory::Instance().Register(NodeType::predicateConstraintNode, new Creator<PredicateConstraintNode>());
    Factory::Instance().Register(NodeType::ccNode, new Creator<CCNode>());
    Factory::Instance().Register(NodeType::isConstraintNode, new Creator<IsConstraintNode>());
    Factory::Instance().Register(NodeType::multiParamConstraintNode, new Creator<MultiParamConstraintNode>());
    Factory::Instance().Register(NodeType::typenameConstraintNode, new Creator<TypenameConstraintNode>());
    Factory::Instance().Register(NodeType::constructorConstraintNode, new Creator<ConstructorConstraintNode>());
    Factory::Instance().Register(NodeType::destructorConstraintNode, new Creator<DestructorConstraintNode>());
    Factory::Instance().Register(NodeType::memberFunctionConstraintNode, new Creator<MemberFunctionConstraintNode>());
    Factory::Instance().Register(NodeType::functionConstraintNode, new Creator<FunctionConstraintNode>());
    Factory::Instance().Register(NodeType::axiomNode, new Creator<AxiomNode>());
    Factory::Instance().Register(NodeType::axiomStatementNode, new Creator<AxiomStatementNode>());
    Factory::Instance().Register(NodeType::conceptIdNode, new Creator<ConceptIdNode>());
    Factory::Instance().Register(NodeType::conceptNode, new Creator<ConceptNode>());
    Factory::Instance().Register(NodeType::functionGroupIdNode, new Creator<FunctionGroupIdNode>());
    Factory::Instance().Register(NodeType::templateParameterNode, new Creator<TemplateParameterNode>());
    Factory::Instance().Register(NodeType::functionNode, new Creator<FunctionNode>());
    Factory::Instance().Register(NodeType::classNode, new Creator<ClassNode>());
    Factory::Instance().Register(NodeType::interfaceNode, new Creator<InterfaceNode>());
    Factory::Instance().Register(NodeType::memberInitializerNode, new Creator<MemberInitializerNode>());
    Factory::Instance().Register(NodeType::baseInitializerNode, new Creator<BaseInitializerNode>());
    Factory::Instance().Register(NodeType::thisInitializerNode, new Creator<ThisInitializerNode>());
    Factory::Instance().Register(NodeType::staticConstructorNode, new Creator<StaticConstructorNode>());
    Factory::Instance().Register(NodeType::constructorNode, new Creator<ConstructorNode>());
    Factory::Instance().Register(NodeType::destructorNode, new Creator<DestructorNode>());
    Factory::Instance().Register(NodeType::memberFunctionNode, new Creator<MemberFunctionNode>());
    Factory::Instance().Register(NodeType::conversionFunctionNode, new Creator<ConversionFunctionNode>());
    Factory::Instance().Register(NodeType::memberVariableNode, new Creator<MemberVariableNode>());
    Factory::Instance().Register(NodeType::aliasNode, new Creator<AliasNode>());
    Factory::Instance().Register(NodeType::namespaceImportNode, new Creator<NamespaceImportNode>());
    Factory::Instance().Register(NodeType::namespaceNode, new Creator<NamespaceNode>());
    Factory::Instance().Register(NodeType::compileUnitNode, new Creator<CompileUnitNode>());
}

void DoneFactory()
{
    Factory::Done();
}

} } // namespace Cm::Ast
