/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_OPT_TYPE_PROPAGATION_GRAPH_INCLUDED
#define CM_OPT_TYPE_PROPAGATION_GRAPH_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Util/CodeFormatter.hpp>
#include <queue>

namespace Cm { namespace Opt {

// Type Propagation Graph Node:

class TpGraphNode
{
public:
    TpGraphNode(Cm::Sym::VariableSymbol* variableSymbol_);
    uint32_t VariableSymbolSid() const { return variableSymbolSid; }
    std::unordered_set<Cm::Sym::ClassTypeSymbol*>& ReachingClasses() { return reachingClasses; }
    void AddTarget(TpGraphNode* target);
    void Print(Cm::Util::CodeFormatter& formatter);
    void PrintReachingSet(Cm::Util::CodeFormatter& formatter);
    void Propagate(std::queue<TpGraphNode*>& workList);
private:
    uint32_t variableSymbolSid;
    std::string variableSymbolFullName;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> reachingClasses;
    std::unordered_set<TpGraphNode*> targets;
    bool visited;
};

// Type Propagation Graph:

class TpGraph
{
public:
    TpGraphNode* GetNode(Cm::Sym::VariableSymbol* variableSymbol);
    void Print(Cm::Util::CodeFormatter& formatter);
    void AddRoot(TpGraphNode* root);
    void Process();
private:
    std::vector<std::unique_ptr<TpGraphNode>> nodes;
    std::unordered_map<uint32_t, TpGraphNode*> nodeMap;
    std::unordered_set<TpGraphNode*> roots;
};

class TpGraphBuilderVisitor : public Cm::BoundTree::Visitor
{
public:
    TpGraphBuilderVisitor(TpGraph& graph_);
    void BeginVisit(Cm::BoundTree::BoundClass& boundClass) override;
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    void EndVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    void Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement) override;
    void Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) override;
    void Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall) override;
    TpGraph& Graph() { return graph; }
    void PrintVirtualCalls(Cm::Util::CodeFormatter& formatter);
private:
    TpGraph& graph;
    Cm::BoundTree::BoundClass* currentClass;
    std::string currentFunction;
    void AddEdgesFromArgumentsToParameters(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::BoundExpressionList& arguments);
    bool inMain;
    TpGraphNode* sourceNode;
    TpGraphNode* entryNode;
    std::stack<TpGraphNode*> sourceNodeStack;
    std::vector<std::pair<std::string, TpGraphNode*>> virtualCalls;
    void PushSourceNode(TpGraphNode* sourceNode_);
    void PopSourceNode();
};

} } // namespace Cm::Opt

#endif // CM_OPT_TYPE_PROPAGATION_GRAPH_INCLUDED
