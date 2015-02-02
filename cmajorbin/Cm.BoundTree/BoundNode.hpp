/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_NODE_INCLUDED
#define CM_BOUND_TREE_BOUND_NODE_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace BoundTree {

class Visitor;

enum class BoundNodeFlags : uint16_t
{
    none = 0,
    argByRef = 1 << 0,
    lvalue = 1 << 1,
    refByValue = 1 << 2,
    constructVariable = 1 << 3,
    genJumpingBoolCode = 1 << 4,
    classObjectArg = 1 << 5,
    genVirtualCall = 1 << 6,
    argIsThisOrBase = 1 << 7,
    argIsTemporary = 1 << 8,
    addrArg = 1 << 9
};

inline BoundNodeFlags operator|(BoundNodeFlags left, BoundNodeFlags right)
{
    return BoundNodeFlags(uint16_t(left) | uint16_t(right));
}

inline BoundNodeFlags operator&(BoundNodeFlags left, BoundNodeFlags right)
{
    return BoundNodeFlags(uint16_t(left) & uint16_t(right));
}

inline BoundNodeFlags operator~(BoundNodeFlags flag)
{
    return BoundNodeFlags(~uint16_t(flag));
}

class BoundNode
{
public:
    BoundNode(Cm::Ast::Node* syntaxNode_);
    virtual ~BoundNode();
    Cm::Ast::Node* SyntaxNode() const { return syntaxNode; }
    virtual bool IsBoundExpressionNode() const { return false;  }
    virtual void Accept(Visitor& visitor) = 0;
    void SetFlag(BoundNodeFlags flag) { flags = flags | flag; }
    bool GetFlag(BoundNodeFlags flag) const { return (flags & flag) != BoundNodeFlags::none; }
    void ResetFlag(BoundNodeFlags flag) { flags = flags & ~flag; }
private:
    BoundNodeFlags flags;
    Cm::Ast::Node* syntaxNode;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_NODE_INCLUDED
