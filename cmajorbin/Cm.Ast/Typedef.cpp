/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Typedef.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

TypedefNode::TypedefNode(const Span& span_) : Node(span_), specifiers(Specifiers::none)
{
}

TypedefNode::TypedefNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_) : Node(span_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_)
{
}

Node* TypedefNode::Clone() const
{
    return new TypedefNode(GetSpan(), specifiers, typeExpr->Clone(), static_cast<IdentifierNode*>(id->Clone()));
}

void TypedefNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    id.reset(reader.ReadIdentifierNode());
}

void TypedefNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
}

} } // namespace Cm::Ast
