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
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Clone.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <algorithm>

namespace Cm { namespace Ast {

NamespaceNode::NamespaceNode(const Span& span_) : Node(span_)
{
}

NamespaceNode::NamespaceNode(const Span& span_, IdentifierNode* id_) : Node(span_), id(id_)
{
    id->SetParent(this);
}

void NamespaceNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

Node* NamespaceNode::Clone(CloneContext& cloneContext) const
{
    NamespaceNode* clone = new NamespaceNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    for (const std::unique_ptr<Node>& member : members)
    {
        if (cloneContext.MakeTestUnits() && member->IsFunctionNode() && (static_cast<FunctionNode*>(member.get())->GetSpecifiers() & Specifiers::unit_test) != Specifiers::none)
        {
            FunctionNode* unitTestFunction = static_cast<FunctionNode*>(member->Clone(cloneContext));
            unitTestFunction->SetParent(const_cast<NamespaceNode*>(this));
            cloneContext.AddUnitTestFunction(unitTestFunction);
        }
        else
        {
            clone->AddMember(member->Clone(cloneContext));
        }
    }
    return clone;
}

void NamespaceNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    members.Read(reader);
    members.SetParent(this);
}

void NamespaceNode::Write(Writer& writer)
{
    writer.Write(id.get());
    members.Write(writer);
}

void NamespaceNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("namespace " + id->ToString());
    formatter.WriteLine("{");
    formatter.IncIndent();
    members.Print(formatter);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void NamespaceNode::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    members.Accept(visitor);
    visitor.EndVisit(*this);
}

std::string NamespaceNode::Name() const
{
    return id->Str();
}

NamespaceNode* NamespaceNode::GetNamespace(const std::string& fullNamespaceName) const
{
    if (fullNamespaceName.empty()) return const_cast<NamespaceNode*>(this);
    std::vector<std::string> components = Cm::Util::Split(fullNamespaceName, '.');
    for (const std::unique_ptr<Node>& member : members)
    {
        if (member->IsNamespaceNode())
        {
            std::vector<std::string> memberComponents = Cm::Util::Split(member->Name(), '.');
            int n = std::min(int(components.size()), int(memberComponents.size()));
            int k = 0;
            for (int i = 0; i < n; ++i)
            {
                if (components[i] == memberComponents[i])
                {
                    k = i;
                }
                else
                {
                    break;
                }
            }
            if (k + 1 == memberComponents.size())
            {
                bool first = true;
                std::string right;
                for (int i = k + 1; i < int(components.size()); ++i)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        right.append(1, '.');
                    }
                    right.append(components[i]);
                }
                return static_cast<NamespaceNode*>(member.get())->GetNamespace(right);
            }
        }
    }
    throw std::runtime_error("namespace '" + fullNamespaceName + "' not found");
}

AliasNode::AliasNode(const Span& span_) : Node(span_)
{
}

AliasNode::AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_) : Node(span_), id(id_), qid(qid_)
{
    id->SetParent(this);
    qid->SetParent(this);
}

Node* AliasNode::Clone(CloneContext& cloneContext) const
{
    return new AliasNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)), static_cast<IdentifierNode*>(qid->Clone(cloneContext)));
}

void AliasNode::Read(Reader& reader)
{
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    qid.reset(reader.ReadIdentifierNode());
    qid->SetParent(this);
}

void AliasNode::Write(Writer& writer)
{
    writer.Write(id.get());
    writer.Write(qid.get());
}

void AliasNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("using " + id->ToString() + " = " + qid->ToString() + ";");
}

void AliasNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NamespaceImportNode::NamespaceImportNode(const Span& span_) : Node(span_)
{
}

NamespaceImportNode::NamespaceImportNode(const Span& span_, IdentifierNode* ns_) : Node(span_), ns(ns_)
{
    ns->SetParent(this);
}

Node* NamespaceImportNode::Clone(CloneContext& cloneContext) const
{
    return new NamespaceImportNode(GetSpan(), static_cast<IdentifierNode*>(ns->Clone(cloneContext)));
}

void NamespaceImportNode::Read(Reader& reader)
{
    ns.reset(reader.ReadIdentifierNode());
    ns->SetParent(this);
}

void NamespaceImportNode::Write(Writer& writer)
{
    writer.Write(ns.get());
}

void NamespaceImportNode::Print(CodeFormatter& formatter)
{
    formatter.WriteLine("using " + ns->ToString() + ";");
}

void NamespaceImportNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
