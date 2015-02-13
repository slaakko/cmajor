/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_COMPILE_UNIT_INCLUDED
#define CM_AST_COMPILE_UNIT_INCLUDED
#include <Cm.Ast/Namespace.hpp>

namespace Cm { namespace Ast {

class Visitor;

class CompileUnitNode : public Node
{
public:
    CompileUnitNode(const Span& span_);
    CompileUnitNode(const Span& span_, const std::string& filePath_);
    NodeType GetNodeType() const override { return NodeType::compileUnitNode; }
    NamespaceNode* GlobalNs() const { return globalNs.get(); }
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
    const std::string& FilePath() const { return filePath; }
private:
    std::string filePath;
    std::unique_ptr<NamespaceNode> globalNs;
};

} } // namespace Cm::Ast

#endif // CM_AST_COMPILE_UNIT_INCLUDED
