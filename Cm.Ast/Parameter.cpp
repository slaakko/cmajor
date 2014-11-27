/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

ParameterNode::ParameterNode(const Span& span_) : Node(span_)
{
}

ParameterNode::ParameterNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_) : Node(span_), typeExpr(typeExpr_), id(id_)
{
}

Node* ParameterNode::Clone() const
{
    return new ParameterNode(GetSpan(), typeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
}

void ParameterNode::Read(Reader& reader)
{
    typeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
}

void ParameterNode::Write(Writer& writer)
{
    writer.Write(typeExpr.get());
    writer.Write(id.get());
}

} } // namespace Cm::Ast
