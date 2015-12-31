/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

IdentifierNode::IdentifierNode(const Span& span_): Node(span_), identifier()
{
}

IdentifierNode::IdentifierNode(const Span& span_, const std::string& identifier_) : Node(span_), identifier(identifier_)
{
}

Node* IdentifierNode::Clone(CloneContext& cloneContext) const
{
    return new IdentifierNode(GetSpan(), identifier);
}

void IdentifierNode::Read(Reader& reader) 
{
    identifier = reader.ReadString();
}

void IdentifierNode::Write(Writer& writer) 
{
    writer.Write(identifier);
}

void IdentifierNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

std::string IdentifierNode::ToString() const
{
    return identifier;
}

} } // namespace Cm::Ast
