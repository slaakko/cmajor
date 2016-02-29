/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_VISITOR_INCLUDED
#define CM_AST_VISITOR_INCLUDED
#include <stack>

namespace Cm { namespace Ast {

class CompileUnitNode;
class AliasNode;
class NamespaceImportNode;
class NamespaceNode;
class FunctionNode;
class ConstantNode;
class EnumTypeNode;
class EnumConstantNode;
class TypedefNode;
class ClassNode;
class InterfaceNode;
class DelegateNode;
class ClassDelegateNode;
class ParameterNode;
class TemplateParameterNode;

class EquivalenceNode;
class ImplicationNode;
class DisjunctionNode;
class ConjunctionNode;
class BitOrNode;
class BitXorNode;
class BitAndNode;
class EqualNode;
class NotEqualNode;
class LessNode;
class GreaterNode;
class LessOrEqualNode;
class GreaterOrEqualNode;
class ShiftLeftNode;
class ShiftRightNode;
class AddNode;
class SubNode;
class MulNode;
class DivNode;
class RemNode;
class PrefixIncNode;
class PrefixDecNode;
class UnaryPlusNode;
class UnaryMinusNode;
class NotNode;
class ComplementNode;
class AddrOfNode;
class DerefNode;
class PostfixIncNode;
class PostfixDecNode;
class DotNode;
class ArrowNode;
class InvokeNode;
class IndexNode;

class BooleanLiteralNode;
class SByteLiteralNode;
class ByteLiteralNode;
class ShortLiteralNode;
class UShortLiteralNode;
class IntLiteralNode;
class UIntLiteralNode;
class LongLiteralNode;
class ULongLiteralNode;
class FloatLiteralNode;
class DoubleLiteralNode;
class CharLiteralNode;
class StringLiteralNode;
class NullLiteralNode;

class BoolNode;
class SByteNode;
class ByteNode;
class ShortNode;
class UShortNode;
class IntNode;
class UIntNode;
class LongNode;
class ULongNode;
class FloatNode;
class DoubleNode;
class CharNode;
class WCharNode;
class UCharNode;
class VoidNode;
class DerivedTypeExprNode;

class SizeOfNode;
class CastNode;
class IsNode;
class AsNode;
class ConstructNode;
class NewNode;
class TemplateIdNode;
class IdentifierNode;
class ThisNode;
class BaseNode;
class TypeNameNode;

class CompoundStatementNode;
class ReturnStatementNode;
class ConditionalStatementNode;
class SwitchStatementNode;
class CaseStatementNode;
class DefaultStatementNode;
class GotoCaseStatementNode;
class GotoDefaultStatementNode;
class WhileStatementNode;
class DoStatementNode;
class RangeForStatementNode;
class ForStatementNode;
class BreakStatementNode;
class ContinueStatementNode;
class GotoStatementNode;
class TypedefStatementNode;
class SimpleStatementNode;
class AssignmentStatementNode;
class ConstructionStatementNode;
class DeleteStatementNode;
class DestroyStatementNode;
class ThrowStatementNode;
class TryStatementNode;
class CatchNode;
class ExitTryStatementNode;
class BeginCatchStatementNode;
class AssertStatementNode;
class CondCompDisjunctionNode;
class CondCompConjunctionNode;
class CondCompNotNode;
class CondCompilationPartNode;
class CondCompPrimaryNode;
class CondCompStatementNode;

class MemberInitializerNode;
class BaseInitializerNode;
class ThisInitializerNode;
class InitializerNodeList;
class StaticConstructorNode;
class ConstructorNode;
class DestructorNode;
class MemberFunctionNode;
class ConversionFunctionNode;
class MemberVariableNode;

class DisjunctiveConstraintNode;
class ConjunctiveConstraintNode;
class WhereConstraintNode;
class IsConstraintNode;
class MultiParamConstraintNode;
class TypenameConstraintNode;
class ConstructorConstraintNode;
class DestructorConstraintNode;
class MemberFunctionConstraintNode;
class FunctionConstraintNode;
class AxiomStatementNode;
class AxiomNode;
class ConceptIdNode;
class ConceptNode;

class SameConstraintNode;
class DerivedConstraintNode;
class ConvertibleConstraintNode;
class ExplicitlyConvertibleConstraintNode;
class CommonConstraintNode;
class NonReferenceTypeConstraintNode;
class SameConceptNode;
class DerivedConceptNode;
class ConvertibleConceptNode;
class ExplicitlyConvertibleConceptNode;
class CommonConceptNode;

class Visitor
{
public:
    Visitor(bool visitBodies_, bool visitExpressions_);
    virtual ~Visitor();

    virtual void BeginVisit(CompileUnitNode& compileUnitNode) {}
    virtual void EndVisit(CompileUnitNode& compileUnitNode) {}
    virtual void Visit(AliasNode& aliasNode) {}
    virtual void Visit(NamespaceImportNode& namespaceImportNode) {}
    virtual void BeginVisit(NamespaceNode& namespaceNode) {}
    virtual void EndVisit(NamespaceNode& namespaceNode) {}
    virtual void BeginVisit(FunctionNode& functionNode) {}
    virtual void EndVisit(FunctionNode& functionNode) {}
    virtual void Visit(ConstantNode& constantNode) {}
    virtual void BeginVisit(EnumTypeNode& enumTypeNode) {}
    virtual void EndVisit(EnumTypeNode& enumTypeNode) {}
    virtual void Visit(EnumConstantNode& enumConstantNode) {}
    virtual void Visit(TypedefNode& typedefNode) {}
    virtual void BeginVisit(ClassNode& classNode) {}
    virtual void EndVisit(ClassNode& classNode) {}
    virtual void BeginVisit(InterfaceNode& interfaceNode) {}
    virtual void EndVisit(InterfaceNode& interfaceNode) {}
    virtual void BeginVisit(DelegateNode& delegateNode) {}
    virtual void EndVisit(DelegateNode& delegateNode) {}
    virtual void BeginVisit(ClassDelegateNode& classDelegateNode) {}
    virtual void EndVisit(ClassDelegateNode& classDelegateNode) {}
    virtual void Visit(ParameterNode& parameterNode) {}
    virtual void Visit(TemplateParameterNode& templateparameterNode) {}

    virtual void BeginVisit(EquivalenceNode& equivalenceNode) {}
    virtual void EndVisit(EquivalenceNode& equivalenceNode) {}
    virtual void BeginVisit(ImplicationNode& implicationNode) {}
    virtual void EndVisit(ImplicationNode& implicationNode) {}
    virtual void BeginVisit(DisjunctionNode& disjunctionNode) {}
    virtual void EndVisit(DisjunctionNode& disjunctionNode) {}
    virtual void BeginVisit(ConjunctionNode& conjunctionNode) {}
    virtual void EndVisit(ConjunctionNode& conjunctionNode) {}
    virtual void BeginVisit(BitOrNode& bitOrNode) {}
    virtual void EndVisit(BitOrNode& bitOrNode) {}
    virtual void BeginVisit(BitXorNode& bitXorNode) {}
    virtual void EndVisit(BitXorNode& bitXorNode) {}
    virtual void BeginVisit(BitAndNode& bitAndNode) {}
    virtual void EndVisit(BitAndNode& bitAndNode) {}
    virtual void BeginVisit(EqualNode& equalNode) {}
    virtual void EndVisit(EqualNode& equalNode) {}
    virtual void BeginVisit(NotEqualNode& notEqualNode) {}
    virtual void EndVisit(NotEqualNode& notEqualNode) {}
    virtual void BeginVisit(LessNode& lessNode) {}
    virtual void EndVisit(LessNode& lessNode) {}
    virtual void BeginVisit(GreaterNode& greaterNode) {}
    virtual void EndVisit(GreaterNode& greaterNode) {}
    virtual void BeginVisit(LessOrEqualNode& lessOrEqualNode) {}
    virtual void EndVisit(LessOrEqualNode& lessOrEqualNode) {}
    virtual void BeginVisit(GreaterOrEqualNode& greaterOrEqualNode) {}
    virtual void EndVisit(GreaterOrEqualNode& greaterOrEqualNode) {}
    virtual void BeginVisit(ShiftLeftNode& shiftLeftNode) {}
    virtual void EndVisit(ShiftLeftNode& shiftLeftNode) {}
    virtual void BeginVisit(ShiftRightNode& shiftRightNode) {}
    virtual void EndVisit(ShiftRightNode& shiftRightNode) {}
    virtual void BeginVisit(AddNode& addNode) {}
    virtual void EndVisit(AddNode& addNode) {}
    virtual void BeginVisit(SubNode& subNode) {}
    virtual void EndVisit(SubNode& subNode) {}
    virtual void BeginVisit(MulNode& mulNode) {}
    virtual void EndVisit(MulNode& mulNode) {}
    virtual void BeginVisit(DivNode& divNode) {}
    virtual void EndVisit(DivNode& divNode) {}
    virtual void BeginVisit(RemNode& remNode) {}
    virtual void EndVisit(RemNode& remNode) {}
    virtual void BeginVisit(PrefixIncNode& prefixIncNode) {}
    virtual void EndVisit(PrefixIncNode& prefixIncNode) {}
    virtual void BeginVisit(PrefixDecNode& prefixDecNode) {}
    virtual void EndVisit(PrefixDecNode& prefixDecNode) {}
    virtual void BeginVisit(UnaryPlusNode& unaryPlusNode) {}
    virtual void EndVisit(UnaryPlusNode& unaryPlusNode) {}
    virtual void BeginVisit(UnaryMinusNode& unaryMinusNode) {}
    virtual void EndVisit(UnaryMinusNode& unaryMinusNode) {}
    virtual void BeginVisit(NotNode& notNode) {}
    virtual void EndVisit(NotNode& notNode) {}
    virtual void BeginVisit(ComplementNode& complementNode) {}
    virtual void EndVisit(ComplementNode& complementNode) {}
    virtual void Visit(AddrOfNode& addrOfNode) {}
    virtual void Visit(DerefNode& derefNode) {}
    virtual void Visit(PostfixIncNode& postfixIncNode) {}
    virtual void Visit(PostfixDecNode& postfixDecNode) {}
    virtual void EndVisit(PostfixDecNode& postfixDecNode) {}
    virtual void BeginVisit(DotNode& dotNode) {}
    virtual void EndVisit(DotNode& dotNode) {}
    virtual void Visit(ArrowNode& arrowNode) {}
    virtual void BeginVisit(InvokeNode& invokeNode) {}
    virtual void EndVisit(InvokeNode& invokeNode) {}
    virtual void Visit(IndexNode& indexNode) {}

    virtual void Visit(SizeOfNode& sizeOfNode) {}
    virtual void Visit(CastNode& castNode) {}
    virtual void Visit(IsNode& isNode) {}
    virtual void Visit(AsNode& asNode) {}
    virtual void Visit(ConstructNode& constructNode) {}
    virtual void Visit(NewNode& newNode) {}
    virtual void Visit(TemplateIdNode& templateIdNode) {}
    virtual void Visit(IdentifierNode& identifierNode) {}
    virtual void Visit(ThisNode& thisNode) {}
    virtual void Visit(BaseNode& baseNode) {}
    virtual void Visit(TypeNameNode& typeNameNode) {}

    virtual void Visit(BooleanLiteralNode& booleanLiteralNode) {}
    virtual void Visit(SByteLiteralNode& sbyteLiteralNode) {}
    virtual void Visit(ByteLiteralNode& byteLiteralNode) {}
    virtual void Visit(ShortLiteralNode& shortLiteralNode) {}
    virtual void Visit(UShortLiteralNode& ushortLiteralNode) {}
    virtual void Visit(IntLiteralNode& intLiteralNode) {}
    virtual void Visit(UIntLiteralNode& uintLiteralNode) {}
    virtual void Visit(LongLiteralNode& longLiteralNode) {}
    virtual void Visit(ULongLiteralNode& ulongLiteralNode) {}
    virtual void Visit(FloatLiteralNode& floatLiteralNode) {}
    virtual void Visit(DoubleLiteralNode& doubleLiteralNode) {}
    virtual void Visit(CharLiteralNode& charLiteralNode) {}
    virtual void Visit(StringLiteralNode& stringLiteralNode) {}
    virtual void Visit(NullLiteralNode& nullLiteralNode) {}

    virtual void Visit(BoolNode& boolNode) {}
    virtual void Visit(SByteNode& sbyteNode) {}
    virtual void Visit(ByteNode& byteNode) {}
    virtual void Visit(ShortNode& shortNode) {}
    virtual void Visit(UShortNode& shortNode) {}
    virtual void Visit(IntNode& intNode) {}
    virtual void Visit(UIntNode& uintNode) {}
    virtual void Visit(LongNode& longNode) {}
    virtual void Visit(ULongNode& ulongNode) {}
    virtual void Visit(FloatNode& floatNode) {}
    virtual void Visit(DoubleNode& doubleNode) {}
    virtual void Visit(CharNode& charNode) {}
    virtual void Visit(WCharNode& wcharNode) {}
    virtual void Visit(UCharNode& ucharNode) {}
    virtual void Visit(VoidNode& voidNode) {}
    virtual void Visit(DerivedTypeExprNode& derivedTypeExprNode) {}

    virtual void BeginVisit(CompoundStatementNode& compoundStatementNode) {}
    virtual void EndVisit(CompoundStatementNode& compoundStatementNode) {}
    virtual void BeginVisit(ReturnStatementNode& returnStatementNode) {}
    virtual void EndVisit(ReturnStatementNode& returnStatementNode) {}
    virtual void BeginVisit(ConditionalStatementNode& conditionalStatementNode) {}
    virtual void EndVisit(ConditionalStatementNode& conditionalStatementNode) {}
    virtual void BeginVisit(SwitchStatementNode& switchStatementNode) {}
    virtual void EndVisit(SwitchStatementNode& switchStatementNode) {}
    virtual void BeginVisit(CaseStatementNode& caseStatementNode) {}
    virtual void EndVisit(CaseStatementNode& caseStatementNode) {}
    virtual void BeginVisit(DefaultStatementNode& defaultStatementNode) {}
    virtual void EndVisit(DefaultStatementNode& defaultStatementNode) {}
    virtual void BeginVisit(GotoCaseStatementNode& gotoCaseStatementNode) {}
    virtual void EndVisit(GotoCaseStatementNode& gotoCaseStatementNode) {}
    virtual void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) {}
    virtual void BeginVisit(WhileStatementNode& whileStatementNode) {}
    virtual void EndVisit(WhileStatementNode& whileStatementNode) {}
    virtual void BeginVisit(DoStatementNode& doStatementNode) {}
    virtual void EndVisit(DoStatementNode& doStatementNode) {}
    virtual void BeginVisit(RangeForStatementNode& rangeForStatementNode) {}
    virtual void EndVisit(RangeForStatementNode& rangeForStatementNode) {}
    virtual void BeginVisit(ForStatementNode& forStatementNode) {}
    virtual void EndVisit(ForStatementNode& forStatementNode) {}
    virtual void Visit(BreakStatementNode& breakStatementNode) {}
    virtual void Visit(ContinueStatementNode& continueStatementNode) {}
    virtual void Visit(GotoStatementNode& gotoStatementNode) {}
    virtual void Visit(TypedefStatementNode& typedefStatementNode) {}
    virtual void BeginVisit(SimpleStatementNode& simpleStatementNode) {}
    virtual void EndVisit(SimpleStatementNode& simpleStatementNode) {}
    virtual void BeginVisit(AssignmentStatementNode& assignmentStatementNode) {}
    virtual void EndVisit(AssignmentStatementNode& assignmentStatementNode) {}
    virtual void BeginVisit(ConstructionStatementNode& constructionStatementNode) {}
    virtual void EndVisit(ConstructionStatementNode& constructionStatementNode) {}
    virtual void BeginVisit(DeleteStatementNode& deleteStatementNode) {}
    virtual void EndVisit(DeleteStatementNode& deleteStatementNode) {}
    virtual void BeginVisit(DestroyStatementNode& destroyStatementNode) {}
    virtual void EndVisit(DestroyStatementNode& destroyStatementNode) {}
    virtual void BeginVisit(ThrowStatementNode& throwStatementNode) {}
    virtual void EndVisit(ThrowStatementNode& throwStatementNode) {}
    virtual void Visit(TryStatementNode& tryStatementNode) {}
    virtual void Visit(CatchNode& catchNode) {}
    virtual void Visit(ExitTryStatementNode& exitTryStatementNode) {}
    virtual void Visit(BeginCatchStatementNode& beginCatchStatementNode) {}
    virtual void Visit(AssertStatementNode& assertStatementNode) {}
    virtual void BeginVisit(CondCompDisjunctionNode& condCompDisjunctionNode) {}
    virtual void EndVisit(CondCompDisjunctionNode& condCompDisjunctionNode) {}
    virtual void BeginVisit(CondCompConjunctionNode& condCompDisjunctionNode) {}
    virtual void EndVisit(CondCompConjunctionNode& condCompDisjunctionNode) {}
    virtual void BeginVisit(CondCompNotNode& condCompNotNode) {}
    virtual void EndVisit(CondCompNotNode& condCompNotNode) {}
    virtual void Visit(CondCompPrimaryNode& condCompPrimaryNode) {}
    virtual void BeginVisit(CondCompilationPartNode& condCompilationPartNode) {}
    virtual void EndVisit(CondCompilationPartNode& condCompilationPartNode) {}
    virtual void Visit(CondCompStatementNode& condCompStatementNode) {}

    virtual void Visit(MemberInitializerNode& memberInitializerNode) {}
    virtual void Visit(BaseInitializerNode& baseInitializerNode) {}
    virtual void Visit(ThisInitializerNode& thisInitializerNode) {}
    virtual void BeginVisit(InitializerNodeList& initializers) {}
    virtual void EndVisit(InitializerNodeList& initializers) {}
    virtual void BeginVisit(StaticConstructorNode& staticConstructorNode) {}
    virtual void EndVisit(StaticConstructorNode& staticConstructorNode) {}
    virtual void BeginVisit(ConstructorNode& constructorNode) {}
    virtual void EndVisit(ConstructorNode& constructorNode) {}
    virtual void BeginVisit(DestructorNode& destructorNode) {}
    virtual void EndVisit(DestructorNode& destructorNode) {}
    virtual void BeginVisit(MemberFunctionNode& memberFunctionNode) {}
    virtual void EndVisit(MemberFunctionNode& memberFunctionNode) {}
    virtual void BeginVisit(ConversionFunctionNode& conversionFunctionNode) {}
    virtual void EndVisit(ConversionFunctionNode& conversionFunctionNode) {}
    virtual void Visit(MemberVariableNode& memberVariableNode) {}

    virtual void Visit(ConceptNode& conceptNode) {}
    virtual void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) {}
    virtual void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) {}
    virtual void BeginVisit(WhereConstraintNode& whereConstraintNode) {}
    virtual void EndVisit(WhereConstraintNode& whereConstraintNode) {}
    virtual void Visit(IsConstraintNode& isConstraintNode) {}
    virtual void Visit(MultiParamConstraintNode& multiParamConstraintNode) {}
    virtual void Visit(TypenameConstraintNode& typenameConstraintNode) {}
    virtual void Visit(ConstructorConstraintNode& constructorConstraintNode) {}
    virtual void Visit(DestructorConstraintNode& destructorConstraintNode) {}
    virtual void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) {}
    virtual void Visit(FunctionConstraintNode& functionConstraintNode) {}
    virtual void Visit(AxiomStatementNode& axiomStatementNode) {}
    virtual void BeginVisit(AxiomNode& axiomNode) {}
    virtual void EndVisit(AxiomNode& axiomNode) {}
    virtual void Visit(ConceptIdNode& conceptIdNode) {}

    virtual void Visit(SameConstraintNode& sameConstraintNode) {}
    virtual void Visit(DerivedConstraintNode& derivedConstraintNode) {}
    virtual void Visit(ConvertibleConstraintNode& convertibleConstraintNode) {}
    virtual void Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) {}
    virtual void Visit(CommonConstraintNode& commonConstraintNode) {}
    virtual void Visit(NonReferenceTypeConstraintNode& nonReferenceTypeConstraintNode) {}
    virtual void Visit(SameConceptNode& sameConceptNode) {}
    virtual void Visit(DerivedConceptNode& derivedConceptNode) {}
    virtual void Visit(ConvertibleConceptNode& convertibleConceptNode) {}
    virtual void Visit(ExplicitlyConvertibleConceptNode& explicitlyConvertibleConceptNode) {}
    virtual void Visit(CommonConceptNode& commonConceptNode) {}

    bool VisitBodies() const { return visitBodies; }
    bool VisitExpressions() const { return visitExpressions; }
    void PushSkipContent();
    void PopSkipContent();
    bool SkipContent() const { return skipContent; }
private:
    bool visitExpressions;
    bool visitBodies;
    bool skipContent;
    std::stack<bool> skipContentStack;
};

} } // namespace Cm::Ast

#endif // CM_AST_VISITOR_INCLUDED
