/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Constant.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Ast {

ConstantNode::ConstantNode(const Span& span_) : Node(span_), specifiers(Specifiers::none)
{
}

ConstantNode::ConstantNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, Node* value_): 
    specifiers(specifiers_), typeExpr(typeExpr_), id(id_), value(value_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
    value->SetParent(this);
}

Node* ConstantNode::Clone(CloneContext& cloneContext) const
{
    return new ConstantNode(GetSpan(), specifiers, typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)), value->Clone(cloneContext));
}

void ConstantNode::Read(Reader& reader)
{
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    value.reset(reader.ReadNode());
    value->SetParent(this);
}

void ConstantNode::Write(Writer& writer)
{
    writer.Write(specifiers);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
    writer.Write(value.get());
}

void ConstantNode::Print(CodeFormatter& formatter) 
{
    std::string s = SpecifierStr(specifiers);
    if (!s.empty())
    {
        s.append(1, ' ');
    }
    s.append(typeExpr->ToString()).append(1, ' ').append(id->ToString()).append(" = ").append(value->ToString()).append(";");
    formatter.WriteLine(s);
}

std::string ConstantNode::Name() const 
{ 
    return id->Str(); 
}

std::string ConstantNode::ToString() const
{ 
    return id->Str(); 
}

void ConstantNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
