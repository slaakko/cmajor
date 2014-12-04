/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Expression.hpp>
#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

EnumTypeNode::EnumTypeNode(const Span& span_) : Node(span_), specifiers(Specifiers::none), parent(nullptr)
{
}

EnumTypeNode::EnumTypeNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_), parent(nullptr)
{
}

Node* EnumTypeNode::Clone() const
{
    EnumTypeNode* clone = new EnumTypeNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone()));
    for (const std::unique_ptr<Node>& constant : constants)
    {
        clone->AddConstant(constant->Clone());
    }
    return clone;
}

void EnumTypeNode::AddConstant(Node* constant)
{
    constant->SetParent(this);
    constants.Add(constant);
}

Node* EnumTypeNode::GetLastConstant()
{
    if (constants.Count() == 0)
    {
        return nullptr;
    }
    else
    {
        return constants.Back();
    }
}

void EnumTypeNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    constants.Read(reader);
}

void EnumTypeNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(id.get());
    constants.Write(writer);
}

void EnumTypeNode::Print(CodeFormatter& formatter)
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(1, '{').append(constants.ToString()).append(1, '}');
    formatter.WriteLine(s);
}

Node* EnumTypeNode::Parent() const
{
    return parent;
}

void EnumTypeNode::SetParent(Node* parent_)
{
    parent = parent_;
}

std::string EnumTypeNode::Name() const 
{
    return id->Str(); 
}

void EnumTypeNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    constants.Accept(visitor);
    visitor.EndVisit(*this);
}

EnumConstantNode::EnumConstantNode(const Span& span_) : Node(span_), parent(nullptr)
{
}

EnumConstantNode::EnumConstantNode(const Span& span_, IdentifierNode* id_, Node* value_) : Node(span_), id(id_), value(value_), parent(nullptr)
{
}

Node* EnumConstantNode::Clone() const
{
    return new EnumConstantNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone()), value->Clone());
}

void EnumConstantNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    value.reset(reader.ReadNode());
}

void EnumConstantNode::Write(Writer& writer)
{
    writer.Write(id.get());
    writer.Write(value.get());
}

std::string EnumConstantNode::ToString() const
{
    return id->ToString() + " = " + value->ToString();
}

Node* EnumConstantNode::Parent() const
{
    return parent;
}

void EnumConstantNode::SetParent(Node* parent_) 
{
    parent = parent_;
}

std::string EnumConstantNode::Name() const 
{
    return id->Str(); 
}

void EnumConstantNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

Node* MakeNextEnumConstantValue(const Span& span, EnumTypeNode* enumType)
{
    Node* lastConstant = enumType->GetLastConstant();
    if (lastConstant)
    {
        Node* lastValue = lastConstant->GetValue();
        if (lastValue)
        {
            Node* clonedValue = lastValue->Clone();
            return new AddNode(span, clonedValue, new SByteLiteralNode(span, 1));
        }
        else
        {
            throw std::runtime_error("last constant returned null value");
        }
    }
    else
    {
        return new SByteLiteralNode(span, 0);
    }
}

} } // namespace Cm::Ast
