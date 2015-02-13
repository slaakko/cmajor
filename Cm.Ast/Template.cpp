/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Template.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

TemplateParameterNodeList::TemplateParameterNodeList()
{
}

void TemplateParameterNodeList::Read(Reader& reader)
{
    uint32_t n = reader.ReadUInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        templateParameterNodes.push_back(std::unique_ptr<TemplateParameterNode>(reader.ReadTemplateParameterNode()));
    }
}

void TemplateParameterNodeList::Write(Writer& writer)
{
    uint32_t n = static_cast<uint32_t>(templateParameterNodes.size());
    writer.Write(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(templateParameterNodes[i].get());
    }
}

std::string TemplateParameterNodeList::ToString() const
{
    std::string s;
    if (templateParameterNodes.empty())
    {
        return s;
    }
    s.append(1, '<');
    bool first = true;
    for (const std::unique_ptr<TemplateParameterNode>& templateParam : templateParameterNodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(templateParam->ToString());
    }
    s.append(1, '>');
    return s;
}

void TemplateParameterNodeList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<TemplateParameterNode>& templateParam : templateParameterNodes)
    {
        templateParam->Accept(visitor);
    }
}

void TemplateParameterNodeList::SetParent(Node* parent)
{
    for (const std::unique_ptr<TemplateParameterNode>& templateParam : templateParameterNodes)
    {
        templateParam->SetParent(parent);
    }
}

TemplateParameterNode::TemplateParameterNode(const Span& span_) : Node(span_)
{
}

TemplateParameterNode::TemplateParameterNode(const Span& span_, IdentifierNode* id_, Node* defaultTemplateArgument_) : Node(span_), id(id_), defaultTemplateArgument(defaultTemplateArgument_)
{
    id->SetParent(this);
    if (defaultTemplateArgument)
    {
        defaultTemplateArgument->SetParent(this);
    }
}

Node* TemplateParameterNode::Clone(CloneContext& cloneContext) const
{
    Cm::Ast::Node* clonedDefaultTemplateargument = nullptr;
    if (defaultTemplateArgument)
    {
        clonedDefaultTemplateargument = defaultTemplateArgument->Clone(cloneContext);
    }
    return new TemplateParameterNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)), clonedDefaultTemplateargument);
}

void TemplateParameterNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    bool hasDefaultTemplateArgument = reader.ReadBool();
    if (hasDefaultTemplateArgument)
    {
        defaultTemplateArgument.reset(reader.ReadNode());
        defaultTemplateArgument->SetParent(this);
    }
}

void TemplateParameterNode::Write(Writer& writer)
{
    writer.Write(id.get());
    bool hasDefaultTemplateArgument = defaultTemplateArgument != nullptr;
    writer.Write(hasDefaultTemplateArgument);
    if (hasDefaultTemplateArgument)
    {
        writer.Write(defaultTemplateArgument.get());
    }
}

void TemplateParameterNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string TemplateParameterNode::ToString() const
{
    std::string s = id->ToString();
    if (defaultTemplateArgument)
    {
        s.append(" = ").append(defaultTemplateArgument->ToString());
    }
    return s;
}

TemplateIdNode::TemplateIdNode(const Span& span_): Node(span_)
{
}

TemplateIdNode::TemplateIdNode(const Span& span_, Node* subject_): Node(span_), subject(subject_)
{
    subject->SetParent(this);
}

Node* TemplateIdNode::Clone(CloneContext& cloneContext) const
{
    TemplateIdNode* clone = new TemplateIdNode(GetSpan(), subject->Clone(cloneContext));
    for (const std::unique_ptr<Node>& templateArgument : templateArguments)
    {
        clone->AddTemplateArgument(templateArgument->Clone(cloneContext));
    }
    return clone;
}

void TemplateIdNode::AddTemplateArgument(Node* templateArgument)
{
    templateArgument->SetParent(this);
    templateArguments.Add(templateArgument);
}

void TemplateIdNode::Read(Reader& reader)
{
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
    templateArguments.Read(reader);
    templateArguments.SetParent(this);
}

void TemplateIdNode::Write(Writer& writer)
{
    writer.Write(subject.get());
    templateArguments.Write(writer);
}

void TemplateIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string TemplateIdNode::ToString() const
{
    std::string s = subject->ToString();
    s.append(1, '<').append(templateArguments.ToString()).append(1, '>');
    return s;
}

} } // namespace Cm::Ast