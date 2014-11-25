/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_IDENTIFIER_INCLUDED
#define CM_AST_IDENTIFIER_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class IdentifierNode : public Node
{
public:
    IdentifierNode(const Span& span_);
    IdentifierNode(const Span& span_, const std::string& identifier_);
    NodeType GetType() const override { return NodeType::identifierNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::string identifier;
};

} } // namespace Cm::Ast

#endif // CM_AST_IDENTIFIER_INCLUDED