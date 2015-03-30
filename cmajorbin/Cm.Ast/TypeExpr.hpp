/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_TYPE_EXPR_INCLUDED
#define CM_AST_TYPE_EXPR_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <memory>

namespace Cm { namespace Ast {

enum class Derivation: uint8_t
{
    none = 0,
    const_ = 1,
    reference = 2,
    rvalueRef = 3,
    pointer = 4,
    leftParen = 5,
    rightParen = 6
};

std::string DerivationStr(Derivation d);

const int maxDerivations = 7;

class DerivationList
{
public:
    DerivationList();
    typedef const Derivation* const_iterator;
    uint8_t NumDerivations() const { return numDerivations; }
    Derivation operator[](int index) const { return derivations[index];  }
    Derivation& operator[](int index) { return derivations[index]; }
    void Add(Derivation derivation);
    void InsertFront(Derivation derivation);
    const_iterator begin() const { return derivations; }
    const_iterator end() const { return &derivations[numDerivations]; }
    void RemoveLastPointer();
    void RemoveReference();
private:
    uint8_t numDerivations;
    Derivation derivations[maxDerivations];
};

std::string MakeDerivedTypeName(const DerivationList& derivations, const std::string& baseTypeFullName);

bool operator==(const DerivationList& left, const DerivationList& right);
bool operator<(const DerivationList& left, const DerivationList& right);

class DerivedTypeExprNode : public Node
{
public:
    DerivedTypeExprNode(const Span& span_);
    DerivedTypeExprNode(const Span& span_, const DerivationList& derivations_, Node* baseTypeExprNode_);
    NodeType GetNodeType() const override { return NodeType::derivedTypeExprNode; }
    Node* Clone(CloneContext& cloneContext) const override;
    const DerivationList& Derivations() const { return derivations; }
    Node* BaseTypeExprNode() const { return baseTypeExprNode.get();  }
    Node* ReleaseBaseTypeExprNode() { return baseTypeExprNode.release();  }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
    void Add(Derivation derivation);
    void AddConst() { Add(Derivation::const_); }
    void AddRvalueRef() { Add(Derivation::rvalueRef); }
    void AddReference() { Add(Derivation::reference); }
    void AddPointer() { Add(Derivation::pointer); }
    void AddLeftParen() { Add(Derivation::leftParen); }
    void AddRightParen() { Add(Derivation::rightParen); }
    void SetBaseTypeExpr(Node* baseTypeExprNode_);
private:
    DerivationList derivations;
    std::unique_ptr<Node> baseTypeExprNode;
};

Node* MakeTypeExprNode(DerivedTypeExprNode* derivedTypeExprNode);

} } // namespace Cm::Ast

#endif // CM_AST_TYPE_EXPR_INCLUDED
