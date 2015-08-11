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
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Concept.hpp>
#include <stdexcept>

namespace Cm { namespace Ast {

EnumTypeNode::EnumTypeNode(const Span& span_) : Node(span_), specifiers(Specifiers::none)
{
}

EnumTypeNode::EnumTypeNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_)
{
    id->SetParent(this);
}

Node* EnumTypeNode::Clone(CloneContext& cloneContext) const
{
    EnumTypeNode* clone = new EnumTypeNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    if (underlyingType)
    {
        clone->SetUnderlyingType(underlyingType->Clone(cloneContext));
    }
    for (const std::unique_ptr<Node>& constant : constants)
    {
        clone->AddConstant(constant->Clone(cloneContext));
    }
    return clone;
}

void EnumTypeNode::SetUnderlyingType(Node* underlyingType_)
{
    underlyingType.reset(underlyingType_);
    underlyingType->SetParent(this);
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
    id->SetParent(this);
    constants.Read(reader);
    constants.SetParent(this);
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

EnumConstantNode::EnumConstantNode(const Span& span_) : Node(span_)
{
}

EnumConstantNode::EnumConstantNode(const Span& span_, IdentifierNode* id_, Node* value_) : Node(span_), id(id_), value(value_)
{
    id->SetParent(this);
    value->SetParent(this);
}

Node* EnumConstantNode::Clone(CloneContext& cloneContext) const
{
    return new EnumConstantNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)), value->Clone(cloneContext));
}

void EnumConstantNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    value.reset(reader.ReadNode());
    value->SetParent(this);
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
            CloneContext cloneContext;
            Node* clonedValue = lastValue->Clone(cloneContext);
            if (enumType->GetUnderlyingType())
            {
                if (enumType->GetUnderlyingType()->IsUnsignedTypeNode())
                {
                    return new AddNode(span, clonedValue, new ByteLiteralNode(span, 1));
                }
            }
            return new AddNode(span, clonedValue, new SByteLiteralNode(span, 1));
        }
        else
        {
            throw std::runtime_error("last constant returned null value");
        }
    }
    else
    {
        if (enumType->GetUnderlyingType())
        {
            if (enumType->GetUnderlyingType()->IsUnsignedTypeNode())
            {
                return new ByteLiteralNode(span, 0);
            }
        }
        return new SByteLiteralNode(span, 0);
    }
}

} } // namespace Cm::Ast
