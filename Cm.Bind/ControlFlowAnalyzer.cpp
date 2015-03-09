/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/ControlFlowAnalyzer.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Bind {

typedef std::unordered_map<std::string, Cm::BoundTree::BoundStatement*> LabelStatementMap;
typedef LabelStatementMap::const_iterator LabelStatementMapIt;

class LabelCollector : public Cm::BoundTree::Visitor
{
public:
    LabelCollector();
    void BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement) override;
    void Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement) override;
    void Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement) override;
    void Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement) override;
    void Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement) override;
    void Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) override;
    void Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(Cm::BoundTree::BoundDestructionStatement& boundDestructionStatement) override;
    void Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement) override;
    void Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement) override;
    void Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) override;
    void Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement) override;
    void BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement) override;
    void BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement) override;
    void BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement) override;
    void BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement) override;
    void Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(Cm::BoundTree::BoundCaseStatement& boundCaseStatement) override;
    void Visit(Cm::BoundTree::BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(Cm::BoundTree::BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(Cm::BoundTree::BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    LabelStatementMap& GetLabelStatementMap() { return labelStatementMap; }
private:
    LabelStatementMap labelStatementMap;
    void CollectLabel(Cm::BoundTree::BoundStatement& statement);
};

class GotoResolver : public Cm::BoundTree::Visitor
{
public:
    GotoResolver(LabelStatementMap& labelStatementMap_);
    void Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement) override;
private:
    LabelStatementMap& labelStatementMap;
};

ControlFlowAnalyzer::ControlFlowAnalyzer() : Cm::BoundTree::Visitor(false)
{
}

void ControlFlowAnalyzer::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->Name() == "@destructor(InputFileStream*)")
    {
        int x = 0;
    }
    if (boundFunction.HasGotos())
    {
        LabelCollector labelCollector;
        boundFunction.Accept(labelCollector);
        GotoResolver gotoResolver(labelCollector.GetLabelStatementMap());
        boundFunction.Accept(gotoResolver);
    }
}

GotoResolver::GotoResolver(LabelStatementMap& labelStatementMap_) : Cm::BoundTree::Visitor(true), labelStatementMap(labelStatementMap_)
{
}

void GotoResolver::Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement)
{
    const std::string& targetLabel = boundGotoStatement.TargetLabel();
    LabelStatementMapIt i = labelStatementMap.find(targetLabel);
    if (i != labelStatementMap.end())
    {
        Cm::BoundTree::BoundStatement* targetStatement = i->second;
        Cm::BoundTree::BoundCompoundStatement* targetCompoundParent = targetStatement->CompoundParent();
        boundGotoStatement.SetTargetStatement(targetStatement);
        boundGotoStatement.SetTargetCompoundParent(targetCompoundParent);
        Cm::BoundTree::BoundCompoundStatement* gotoCompoundParent = boundGotoStatement.CompoundParent();
        while (gotoCompoundParent && gotoCompoundParent != targetCompoundParent)
        {
            gotoCompoundParent = gotoCompoundParent->CompoundParent();
        }
        if (!gotoCompoundParent)
        {
            Cm::Parsing::Span refSpan;
            if (targetStatement->SyntaxNode())
            {
                refSpan = targetStatement->SyntaxNode()->GetSpan();
            }
            throw Cm::Core::Exception("goto target '" + targetLabel + "' not in enclosing block", boundGotoStatement.SyntaxNode()->GetSpan(), refSpan);
        }
    }
    else
    {
        throw Cm::Core::Exception("goto target '" + targetLabel + "' not found", boundGotoStatement.SyntaxNode()->GetSpan());
    }
}

LabelCollector::LabelCollector() : Cm::BoundTree::Visitor(true)
{
}

void LabelCollector::CollectLabel(Cm::BoundTree::BoundStatement& statement)
{
    const std::string& label = statement.Label();
    if (!label.empty())
    {
        LabelStatementMapIt i = labelStatementMap.find(label);
        if (i == labelStatementMap.end())
        {
            labelStatementMap[label] = &statement;
        }
        else
        {
            Cm::Parsing::Span refSpan;
            if (i->second->SyntaxNode())
            {
                refSpan = i->second->SyntaxNode()->GetSpan();
            }
            throw Cm::Core::Exception("duplicate label '" + label + "'", statement.SyntaxNode()->GetSpan(), refSpan);
        }
    }
}

void LabelCollector::BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement)
{
    CollectLabel(boundCompoundStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement)
{
    CollectLabel(boundReceiveStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement)
{
    CollectLabel(boundInitClassObjectStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement)
{
    CollectLabel(boundInitVPtrStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement)
{
    CollectLabel(boundInitMemberVariableStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement)
{
    CollectLabel(boundFunctionCallStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement)
{
    CollectLabel(boundReturnStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    CollectLabel(boundConstructionStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundDestructionStatement& boundDestructionStatement)
{
    CollectLabel(boundDestructionStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    CollectLabel(boundAssignmentStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement)
{
    CollectLabel(boundSimpleStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement)
{
    CollectLabel(boundBreakStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement)
{
    CollectLabel(boundContinueStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement)
{
    CollectLabel(boundGotoStatement);
}

void LabelCollector::BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement)
{
    CollectLabel(boundConditionalStatement);
}

void LabelCollector::BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement)
{
    CollectLabel(boundWhileStatement);
}

void LabelCollector::BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement)
{
    CollectLabel(boundDoStatement);
}

void LabelCollector::BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement)
{
    CollectLabel(boundForStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement)
{
    CollectLabel(boundSwitchStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundCaseStatement& boundCaseStatement)
{
    CollectLabel(boundCaseStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundDefaultStatement& boundDefaultStatement)
{
    CollectLabel(boundDefaultStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundGotoCaseStatement& boundGotoCaseStatement)
{
    CollectLabel(boundGotoCaseStatement);
}

void LabelCollector::Visit(Cm::BoundTree::BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    CollectLabel(boundGotoDefaultStatement);
}

} } // namespace Cm::Bind
