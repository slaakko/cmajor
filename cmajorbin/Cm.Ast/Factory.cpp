/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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
}

void DoneFactory()
{
    Factory::Done();
}

} } // namespace Cm::Ast
