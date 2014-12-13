/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_TEMPLATE_NAME_INCLUDED
#define CM_AST_TEMPLATE_NAME_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class TemplateParameterNode;

class TemplateParameterNodeList
{
public:
    typedef std::vector<std::unique_ptr<TemplateParameterNode>>::const_iterator const_iterator;
    TemplateParameterNodeList();
    const_iterator begin() const { return templateParameterNodes.begin(); }
    const_iterator end() const { return templateParameterNodes.end(); }
    int Count() const { return int(templateParameterNodes.size()); }
    TemplateParameterNode* operator[](int index) const { return templateParameterNodes[index].get(); }
    TemplateParameterNode* Back() const { return templateParameterNodes.back().get(); }
    void Add(TemplateParameterNode* templateParameter) { templateParameterNodes.push_back(std::unique_ptr<TemplateParameterNode>(templateParameter)); }
    void Read(Reader& reader);
    void Write(Writer& writer);
    std::string ToString() const;
    void Accept(Visitor& visitor);
    void SetParent(Node* parent);
private:
    std::vector<std::unique_ptr<TemplateParameterNode>> templateParameterNodes;
};

class TemplateParameterNode : public Node
{
public:
    TemplateParameterNode(const Span& span_);
    TemplateParameterNode(const Span& span_, IdentifierNode* id_, Node* defaultTemplateArgument_);
    NodeType GetNodeType() const override { return NodeType::templateParameterNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
    IdentifierNode* Id() const { return id.get(); }
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> defaultTemplateArgument;
};

class TemplateIdNode : public Node
{
public:
    TemplateIdNode(const Span& span_);
    TemplateIdNode(const Span& span_, Node* subject_);
    NodeType GetNodeType() const override { return NodeType::templateIdNode; }
    Node* Clone() const override;
    void AddTemplateArgument(Node* templateArgument);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
    Node* Subject() const { return subject.get(); }
    const NodeList& TemplateArguments() const { return templateArguments; }
private:
    std::unique_ptr<Node> subject;
    NodeList templateArguments;
};

} } // namespace Cm::Ast

#endif // CM_AST_TEMPLATE_NAME_INCLUDED
