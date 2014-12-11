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
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Concept.hpp>
#include <Cm.Ast/Template.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/CompileUnit.hpp>

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
    if (node->GetNodeType() != NodeType::identifierNode)
    {
        throw std::runtime_error("identifier node expected");
    }
    return static_cast<IdentifierNode*>(node);
}

ParameterNode* Reader::ReadParameterNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::parameterNode)
    {
        throw std::runtime_error("parameter node expected");
    }
    return static_cast<ParameterNode*>(node);
}

TemplateParameterNode* Reader::ReadTemplateParameterNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::templateParameterNode)
    {
        throw std::runtime_error("template parameter node expected");
    }
    return static_cast<TemplateParameterNode*>(node);
}

WhereConstraintNode* Reader::ReadWhereConstraintNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::whereConstraintNode)
    {
        throw std::runtime_error("where constraint node expected");
    }
    return static_cast<WhereConstraintNode*>(node);
}

AxiomStatementNode* Reader::ReadAxiomStatementNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::axiomStatementNode)
    {
        throw std::runtime_error("axiom statement node expected");
    }
    return static_cast<AxiomStatementNode*>(node);
}

InitializerNode* Reader::ReadInitializerNode()
{
    Node* node = ReadNode();
    if (node->IsInitializerNode())
    {
        return static_cast<InitializerNode*>(node);
    }
    else
    {
        throw std::runtime_error("initializer node expected");
    }
}

NamespaceNode* Reader::ReadNamespaceNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::namespaceNode)
    {
        throw std::runtime_error("namespace node expected");
    }
    return static_cast<NamespaceNode*>(node);
}

CompileUnitNode* Reader::ReadCompileUnitNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::compileUnitNode)
    {
        throw std::runtime_error("compile unit node expected");
    }
    return static_cast<CompileUnitNode*>(node);
}

ConstraintNode* Reader::ReadConstraintNode()
{
    Node* node = ReadNode();
    if (node->IsConstraintNode())
    {
        return static_cast<ConstraintNode*>(node);
    }
    else
    {
        throw std::runtime_error("constraint node expected");
    }
}

FunctionGroupIdNode* Reader::ReadFunctionGroupIdNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::functionGroupIdNode)
    {
        throw std::runtime_error("function group identifier node expected");
    }
    return static_cast<FunctionGroupIdNode*>(node);
}

LabelNode* Reader::ReadLabelNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::labelNode)
    {
        throw std::runtime_error("label node expected");
    }
    return static_cast<LabelNode*>(node);
}

StatementNode* Reader::ReadStatementNode()
{
    Node* node = ReadNode();
    if (node->IsStatementNode())
    {
        return static_cast<StatementNode*>(node);
    }
    else
    {
        throw std::runtime_error("statement node expected");
    }
}

CompoundStatementNode* Reader::ReadCompoundStatementNode()
{
    Node* node = ReadNode();
    if (node->IsCompoundStatementNode())
    {
        return static_cast<CompoundStatementNode*>(node);
    }
    else
    {
        throw std::runtime_error("compound statement node expected");
    }
}

CondCompExprNode* Reader::ReadCondCompExprNode()
{
    Node* node = ReadNode();
    if (node->IsCondCompExprNode())
    {
        return static_cast<CondCompExprNode*>(node);
    }
    else
    {
        throw std::runtime_error("conditional compilation expression node expected");
    }
}

CondCompSymbolNode* Reader::ReadCondCompSymbolNode()
{
    Node* node = ReadNode();
    if (node->IsCondCompSymbolNode())
    {
        return static_cast<CondCompSymbolNode*>(node);
    }
    else
    {
        throw std::runtime_error("conditional compilation symbol node expected");
    }
}

CondCompilationPartNode* Reader::ReadCondCompPartNode()
{
    Node* node = ReadNode();
    if (node->IsCondCompPartNode())
    {
        return static_cast<CondCompilationPartNode*>(node);
    }
    else
    {
        throw std::runtime_error("conditional compilation part node expected");
    }
}

ConceptIdNode* Reader::ReadConceptIdNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() != NodeType::conceptIdNode)
    {
        throw std::runtime_error("concept identifier node expected");
    }
    return static_cast<ConceptIdNode*>(node);
}

std::string Reader::ReadString()
{
    return binaryReader.ReadString();
}

bool Reader::ReadBool()
{
    return binaryReader.ReadBool();
}

char Reader::ReadChar()
{
    return binaryReader.ReadChar();
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

Specifiers Reader::ReadSpecifiers()
{
    uint16_t s = ReadUShort();
    return *reinterpret_cast<Specifiers*>(&s);
}

} } // namespace Cm::Ast
