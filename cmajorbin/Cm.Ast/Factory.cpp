/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Ast/Factory.hpp>
#include <Cm.Ast/BasicType.hpp>

namespace Cm { namespace Ast {

NodeCreator::~NodeCreator()
{
}

template<typename T>
class Creator: public NodeCreator
{
public:
    virtual Node* CreateNode() 
    {
        return new T();
    }
};

void Factory::Init()
{
    instance = std::unique_ptr<Factory>(new Factory());
}

Factory::Factory()
{
    creators.resize(int(NodeType::maxNode));
}

void Factory::Register(NodeType nodeType, NodeCreator* creator)
{
    creators[int(nodeType)] = std::unique_ptr<NodeCreator>(creator);
}

Node* Factory::CreateNode(NodeType nodeType)
{
    Node* node = creators[int(nodeType)]->CreateNode();
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
}

} } // namespace Cm::Ast
