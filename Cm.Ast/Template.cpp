/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Template.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

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