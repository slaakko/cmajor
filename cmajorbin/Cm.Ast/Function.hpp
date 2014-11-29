/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_FUNCTION_INCLUDED
#define CM_AST_FUNCTION_INCLUDED
#include <Cm.Ast/Parameter.hpp>

namespace Cm { namespace Ast {

class FunctionGroupIdNode : public Node
{
public:
    FunctionGroupIdNode(const Span& span_);
    FunctionGroupIdNode(const Span& span_, const std::string& functionGroupId_);
    NodeType GetNodeType() const override { return NodeType::functionGroupIdNode; }
    Node* Clone() const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::string functionGroupId;
};

} } // namespace Cm::Ast

#endif // CM_AST_FUNCTION_INCLUDED
