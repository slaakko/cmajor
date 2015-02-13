/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/CompileUnit.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

CompileUnitNode::CompileUnitNode(const Span& span_) : Node(span_)
{
}

CompileUnitNode::CompileUnitNode(const Span& span_, const std::string& filePath_) : Node(span_), filePath(filePath_), globalNs(new NamespaceNode(span_, new IdentifierNode(span_, "")))
{
}

Node* CompileUnitNode::Clone(CloneContext& cloneContext) const
{
    CompileUnitNode* clone = new CompileUnitNode(GetSpan(), filePath);
    clone->globalNs.reset(static_cast<NamespaceNode*>(globalNs->Clone(cloneContext)));
    return clone;
}

void CompileUnitNode::Read(Reader& reader)
{
    filePath = reader.ReadString();
    globalNs.reset(reader.ReadNamespaceNode());
}

void CompileUnitNode::Write(Writer& writer)
{
    writer.Write(filePath);
    writer.Write(globalNs.get());
}

void CompileUnitNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("> " + filePath);
    globalNs->Print(formatter);
}

void CompileUnitNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    globalNs->Accept(visitor);
    visitor.EndVisit(*this);
}

} } // namespace Cm::Ast