/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_BOUND_FUNCTION_INCLUDED
#define CM_BOUND_TREE_BOUND_FUNCTION_INCLUDED
#include <Cm.BoundTree/BoundStatement.hpp>

namespace Cm { namespace BoundTree {

class BoundFunction : public BoundNode
{
public:
    BoundFunction(Cm::Ast::Node* syntaxNode_, Cm::Sym::FunctionSymbol* functionSymbol_);
    Cm::Sym::FunctionSymbol* GetFunctionSymbol() const { return functionSymbol; }
    void SetBody(BoundCompoundStatement* body_);
    BoundCompoundStatement* Body() const { return body.get(); }
    void AddLocalVariable(Cm::Sym::LocalVariableSymbol* localVariable);
    std::string GetNextTempVariableName();
    Cm::Sym::LocalVariableSymbol* CreateTempLocalVariable(Cm::Sym::TypeSymbol* type);
    const std::vector<Cm::Sym::LocalVariableSymbol*>& LocalVariables() const { return localVariables; }
    void SetClassObjectLayoutFunIndex(int classObjectLayoutFunIndex_) { classObjectLayoutFunIndex = classObjectLayoutFunIndex_; }
    int GetClassObjectLayoutFunIndex() const { return classObjectLayoutFunIndex; }
    void Accept(Visitor& visitor) override;
    void SetHasGotos() { hasGotos = true; }
    bool HasGotos() const { return hasGotos; }
    int GetNextCatchId() { return nextCatchId++; }
    void Own(Cm::Ast::Node* syntaxNode);
private:
    std::unique_ptr<BoundCompoundStatement> body;
    Cm::Sym::FunctionSymbol* functionSymbol;
    std::vector<Cm::Sym::LocalVariableSymbol*> localVariables;
    int classObjectLayoutFunIndex;
    std::vector<std::unique_ptr<Cm::Sym::LocalVariableSymbol>> temporaries;
    bool hasGotos;
    int nextCatchId;
    std::vector<std::unique_ptr<Cm::Ast::Node>> syntaxNodes;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_BOUND_FUNCTION_INCLUDED
