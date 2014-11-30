/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>

namespace Cm { namespace Ast {

AliasNode::AliasNode(const Span& span_) : Node(span_)
{
}

AliasNode::AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_) : Node(span_), id(id_), qid(qid_)
{
}

Node* AliasNode::Clone() const
{
    return new AliasNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone()), static_cast<IdentifierNode*>(qid->Clone()));
}

void AliasNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    qid.reset(reader.ReadIdentifierNode());
}

void AliasNode::Write(Writer& writer)
{
    writer.Write(id.get());
    writer.Write(qid.get());
}

NamespaceImportNode::NamespaceImportNode(const Span& span_) : Node(span_)
{
}

NamespaceImportNode::NamespaceImportNode(const Span& span_, IdentifierNode* ns_) : Node(span_), ns(ns_)
{
}

Node* NamespaceImportNode::Clone() const
{
    return new NamespaceImportNode(GetSpan(), static_cast<IdentifierNode*>(ns->Clone()));
}

void NamespaceImportNode::Read(Reader& reader)
{
    ns.reset(reader.ReadIdentifierNode());
}

void NamespaceImportNode::Write(Writer& writer)
{
    writer.Write(ns.get());
}

} } // namespace Cm::Ast
