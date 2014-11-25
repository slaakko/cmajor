/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Node.hpp>
#include <Cm.Ast/Factory.hpp>
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Ast {

Reader::Reader(const std::string& fileName) : binaryReader(fileName)
{
}

NodeType Reader::ReadNodeType()
{
    uint8_t x = binaryReader.ReadByte();
    return *reinterpret_cast<NodeType*>(&x);
}

Span Reader::ReadSpan()
{
    Span span;
    binaryReader.Read(&span, sizeof(span));
    return span;
}

Node* Reader::ReadNode()
{
    NodeType nodeType = ReadNodeType();
    Span span = ReadSpan();
    Node* node = Factory::Instance().CreateNode(nodeType, span);
    node->Read(*this);
    return node;
}

IdentifierNode* Reader::ReadIdentifierNode()
{
    Node* node = ReadNode();
    if (node->GetType() != NodeType::identifierNode)
    {
        throw std::runtime_error("identifier node expected");
    }
    return static_cast<IdentifierNode*>(node);
}

std::string Reader::ReadString()
{
    return binaryReader.ReadString();
}

bool Reader::ReadBool()
{
    return binaryReader.ReadBool();
}

int8_t Reader::ReadSByte()
{
    return binaryReader.ReadSByte();
}

uint8_t Reader::ReadByte()
{
    return binaryReader.ReadByte();
}

int16_t Reader::ReadShort()
{
    return binaryReader.ReadShort();
}

int16_t Reader::ReadUShort()
{
    return binaryReader.ReadUShort();
}

int32_t Reader::ReadInt()
{
    return binaryReader.ReadInt();
}

uint32_t Reader::ReadUInt()
{
    return binaryReader.ReadUInt();
}

int64_t Reader::ReadLong()
{
    return binaryReader.ReadLong();
}

uint64_t Reader::ReadULong()
{
    return binaryReader.ReadULong();
}

float Reader::ReadFloat()
{
    return binaryReader.ReadFloat();
}

double Reader::ReadDouble()
{
    return binaryReader.ReadDouble();
}

char Reader::ReadChar()
{
    return binaryReader.ReadChar();
}

Derivation Reader::ReadDerivation()
{
    uint8_t d = ReadByte();
    return *reinterpret_cast<Derivation*>(&d);
}

DerivationList Reader::ReadDerivationList()
{
    DerivationList derivationList;
    uint8_t numDerivations = ReadByte();
    for (uint8_t i = 0; i < numDerivations; ++i)
    {
        derivationList.Add(ReadDerivation());
    }
    return derivationList;
}

} } // namespace Cm::Ast
