/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_VISITOR_INCLUDED
#define CM_BOUND_TREE_VISITOR_INCLUDED

namespace Cm { namespace BoundTree {

class BoundLiteral;
class BoundConstant;
class BoundLocalVariable;
class BoundMemberVariable;
class BoundConversion;
class BoundCast;
class BoundUnaryOp;
class BoundBinaryOp;
class BoundFunctionCall;
class BoundDisjunction;
class BoundConjunction;

class BoundCompoundStatement;
class BoundReturnStatement;
class BoundConstructionStatement;
class BoundAssignmentStatement;
class BoundThrowStatement;
class BoundSimpleStatement;
class BoundSwitchStatement;
class BoundBreakStatement;
class BoundContinueStatement;
class BoundConditionalStatement;
class BoundDoStatement;
class BoundWhileStatement;
class BoundForStatement;
class BoundTryStatement;

class BoundFunction;
class BoundClass;

class Visitor
{
public:
    Visitor(bool visitFunctionBody_);
    virtual ~Visitor();
    virtual void Visit(BoundLiteral& boundLiteral) {}
    virtual void Visit(BoundConstant& boundConstant) {}
    virtual void Visit(BoundLocalVariable& boundLocalVariable) {}
    virtual void Visit(BoundMemberVariable& boundMemberVariable) {}
    virtual void Visit(BoundConversion& boundConversion) {}
    virtual void Visit(BoundCast& boundCast) {}
    virtual void Visit(BoundUnaryOp& boundUnaryOp) {}
    virtual void Visit(BoundBinaryOp& boundBinaryOp) {}
    virtual void Visit(BoundFunctionCall& boundFunctionCall) {}
    virtual void Visit(BoundDisjunction& boundDisjunction) {}
    virtual void Visit(BoundConjunction& boundConjunction) {}

    virtual void Visit(BoundCompoundStatement& boundCompoundStatement) {}
    virtual void Visit(BoundReturnStatement& boundReturnStatement) {}
    virtual void Visit(BoundConstructionStatement& boundConstructionStatement) {}
    virtual void Visit(BoundAssignmentStatement& boundAssignmentStatement) {}
    virtual void Visit(BoundThrowStatement& boundThrowStatement) {}
    virtual void Visit(BoundSimpleStatement& boundSimpleStatement) {}
    virtual void Visit(BoundSwitchStatement& boundSwitchStatement) {}
    virtual void Visit(BoundBreakStatement& boundBreakStatement) {}
    virtual void Visit(BoundContinueStatement& boundContinueStatement) {}
    virtual void Visit(BoundConditionalStatement& boundConditionalStatement) {}
    virtual void Visit(BoundDoStatement& boundDoStatement) {}
    virtual void Visit(BoundWhileStatement& boundWhileStatement) {}
    virtual void Visit(BoundForStatement& boundForStatement) {}
    virtual void Visit(BoundTryStatement& boundTryStatement) {}

    virtual void BeginVisit(BoundFunction& boundFunction) {}
    virtual void EndVisit(BoundFunction& boundFunction) {}

    virtual void Visit(BoundClass& boundClass) {}

    bool VisitFunctionBody() const { return visitFunctionBody; }
private:
    bool visitFunctionBody;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_VISITOR_INCLUDED
