/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Interface.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

InterfaceNode::InterfaceNode(const Span& span_) : Node(span_), compileUnit(nullptr)
{
}

InterfaceNode::InterfaceNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), id(id_), compileUnit(nullptr)
{
}

void InterfaceNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

Node* InterfaceNode::Clone(CloneContext& cloneContext) const
{
    InterfaceNode* clone = new InterfaceNode(GetSpan(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& member : members)
    {
        clone->AddMember(member->Clone(cloneContext));
    }
    return clone;
}

void InterfaceNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    members.Read(reader);
    members.SetParent(this);
}

void InterfaceNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(id.get());
    members.Write(writer);
}

void InterfaceNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        members.Accept(visitor);
    }
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast
