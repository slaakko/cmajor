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
    Factory::Instance().Register(NodeType::identifierNode, new Creator<IdentifierNode>());
}

void DoneFactory()
{
    Factory::Done();
}

} } // namespace Cm::Ast
