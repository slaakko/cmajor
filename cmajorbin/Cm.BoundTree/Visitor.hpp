/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BOUND_TREE_VISITOR_INCLUDED
#define CM_BOUND_TREE_VISITOR_INCLUDED
#include <stack>

namespace Cm { namespace BoundTree {

class BoundCompileUnit;

class BoundLiteral;
class BoundStringLiteral;
class BoundConstant;
class BoundEnumConstant;
class BoundLocalVariable;
class BoundMemberVariable;
class BoundParameter;
class BoundConversion;
class BoundCast;
class BoundUnaryOp;
class BoundBinaryOp;
class BoundFunctionCall;
class BoundDisjunction;
class BoundConjunction;
class BoundPostfixIncDecExpr;

class BoundStatement;
class BoundCompoundStatement;
class BoundReceiveStatement;
class BoundInitClassObjectStatement;
class BoundInitVPtrStatement;
class BoundInitMemberVariableStatement;
class BoundFunctionCallStatement;
class BoundReturnStatement;
class BoundConstructionStatement;
class BoundAssignmentStatement;
class BoundThrowStatement;
class BoundSimpleStatement;
class BoundSwitchStatement;
class BoundCaseStatement;
class BoundDefaultStatement;
class BoundBreakStatement;
class BoundContinueStatement;
class BoundGotoCaseStatement;
class BoundGotoDefaultStatement;
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

    virtual void BeginVisit(BoundCompileUnit& compileUnit) {}
    virtual void EndVisit(BoundCompileUnit& compileUnit) {}

    virtual void Visit(BoundLiteral& boundLiteral) {}
    virtual void Visit(BoundStringLiteral& boundStringLiteral) {}
    virtual void Visit(BoundConstant& boundConstant) {}
    virtual void Visit(BoundEnumConstant& boundEnumConstant) {}
    virtual void Visit(BoundLocalVariable& boundLocalVariable) {}
    virtual void Visit(BoundMemberVariable& boundMemberVariable) {}
    virtual void Visit(BoundParameter& boundParameter) {}
    virtual void Visit(BoundConversion& boundConversion) {}
    virtual void Visit(BoundCast& boundCast) {}
    virtual void Visit(BoundUnaryOp& boundUnaryOp) {}
    virtual void Visit(BoundBinaryOp& boundBinaryOp) {}
    virtual void Visit(BoundFunctionCall& boundFunctionCall) {}
    virtual void Visit(BoundDisjunction& boundDisjunction) {}
    virtual void Visit(BoundConjunction& boundConjunction) {}
    virtual void Visit(BoundPostfixIncDecExpr& boundPostfixIncDecExpr) {}

    void VisitStatement(BoundStatement& statement);
    virtual void BeginVisitStatement(BoundStatement& statement) {}
    virtual void EndVisitStatement(BoundStatement& statement) {}
    virtual void BeginVisit(BoundCompoundStatement& boundCompoundStatement) {}
    virtual void EndVisit(BoundCompoundStatement& boundCompoundStatement) {}
    virtual void Visit(BoundReceiveStatement& boundReceiveStatement) {}
    virtual void Visit(BoundInitClassObjectStatement& boundInitClassObjectStatement) {}
    virtual void Visit(BoundInitVPtrStatement& boundInitVPtrStatement) {}
    virtual void Visit(BoundInitMemberVariableStatement& boundInitMemberVariableStatement) {}
    virtual void Visit(BoundFunctionCallStatement& boundFunctionCallStatement) {}
    virtual void Visit(BoundReturnStatement& boundReturnStatement) {}
    virtual void Visit(BoundConstructionStatement& boundConstructionStatement) {}
    virtual void Visit(BoundAssignmentStatement& boundAssignmentStatement) {}
    virtual void Visit(BoundThrowStatement& boundThrowStatement) {}
    virtual void Visit(BoundSimpleStatement& boundSimpleStatement) {}
    virtual void Visit(BoundSwitchStatement& boundSwitchStatement) {}
    virtual void Visit(BoundCaseStatement& boundCaseStatement) {}
    virtual void Visit(BoundDefaultStatement& boundDefaultStatement) {}
    virtual void Visit(BoundBreakStatement& boundBreakStatement) {}
    virtual void Visit(BoundContinueStatement& boundContinueStatement) {}
    virtual void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) {}
    virtual void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) {}
    virtual void BeginVisit(BoundConditionalStatement& boundConditionalStatement) {}
    virtual void EndVisit(BoundConditionalStatement& boundConditionalStatement) {}
    virtual void BeginVisit(BoundWhileStatement& boundWhileStatement) {}
    virtual void EndVisit(BoundWhileStatement& boundWhileStatement) {}
    virtual void BeginVisit(BoundDoStatement& boundDoStatement) {}
    virtual void EndVisit(BoundDoStatement& boundDoStatement) {}
    virtual void BeginVisit(BoundForStatement& boundForStatement) {}
    virtual void EndVisit(BoundForStatement& boundForStatement) {}
    virtual void Visit(BoundTryStatement& boundTryStatement) {}

    virtual void BeginVisit(BoundFunction& boundFunction) {}
    virtual void EndVisit(BoundFunction& boundFunction) {}

    virtual void BeginVisit(BoundClass& boundClass) {}
    virtual void EndVisit(BoundClass& boundClass) {}

    bool VisitFunctionBody() const { return visitFunctionBody; }
    void PushSkipContent();
    void PopSkipContent();
    bool SkipContent() const { return skipContent; }
private:
    bool visitFunctionBody;
    bool skipContent;
    std::stack<bool> skipContentStack;
};

} } // namespace Cm::BoundTree

#endif // CM_BOUND_TREE_VISITOR_INCLUDED
