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

class TemplateIdNode : public Node
{
public:
    TemplateIdNode(const Span& span_);
    TemplateIdNode(const Span& span_, Node* subject_);
    NodeType GetType() const override { return NodeType::templateIdNode; }
    Node* Clone() const override;
    void AddTemplateArgument(Node* templateArgument);
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::unique_ptr<Node> subject;
    NodeList templateArguments;
};

} } // namespace Cm::Ast

#endif // CM_AST_TEMPLATE_NAME_INCLUDED
