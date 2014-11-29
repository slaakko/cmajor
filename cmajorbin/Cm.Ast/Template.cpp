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

TemplateParameterNode::TemplateParameterNode(const Span& span_) : Node(span_)
{
}

TemplateParameterNode::TemplateParameterNode(const Span& span_, IdentifierNode* id_, Node* defaultTemplateArgument_) : Node(span_), id(id_), defaultTemplateArgument(defaultTemplateArgument_)
{
}

Node* TemplateParameterNode::Clone() const
{
    return new TemplateParameterNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone()), defaultTemplateArgument->Clone());
}

void TemplateParameterNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    bool hasDefaultTemplateArgument = reader.ReadBool();
    if (hasDefaultTemplateArgument)
    {
        defaultTemplateArgument.reset(reader.ReadNode());
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

TemplateIdNode::TemplateIdNode(const Span& span_): Node(span_)
{
}

TemplateIdNode::TemplateIdNode(const Span& span_, Node* subject_): Node(span_), subject(subject_)
{
}

Node* TemplateIdNode::Clone() const
{
    TemplateIdNode* clone = new TemplateIdNode(GetSpan(), subject->Clone());
    for (const std::unique_ptr<Node>& templateArgument : templateArguments)
    {
        clone->AddTemplateArgument(templateArgument->Clone());
    }
    return clone;
}

void TemplateIdNode::AddTemplateArgument(Node* templateArgument)
{
    templateArguments.Add(templateArgument);
}

void TemplateIdNode::Read(Reader& reader)
{
    subject.reset(reader.ReadNode());
    templateArguments.Read(reader);
}

void TemplateIdNode::Write(Writer& writer)
{
    writer.Write(subject.get());
    templateArguments.Write(writer);
}

} } // namespace Cm::Ast