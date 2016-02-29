/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_INTERFACE_INCLUDED
#define CM_AST_INTERFACE_INCLUDED
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Ast {

class InterfaceNode : public Node
{
public:
    InterfaceNode(const Span& span_);
    InterfaceNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    bool IsInterfaceNode() const override { return true; }
    NodeType GetNodeType() const override { return NodeType::interfaceNode; }
    void AddMember(Node* member);
    Node* Clone(CloneContext& cloneContext) const override;
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    void Accept(Visitor& visitor) override;
    void SetCompileUnit(CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    CompileUnitNode* GetCompileUnit() const { return compileUnit; }
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    NodeList members;
    CompileUnitNode* compileUnit;
};

} } // namespace Cm::Ast

#endif // CM_AST_INTERFACE_INCLUDED
