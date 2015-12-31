/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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

std::string ToNodeLabel(const std::string& s);

// Type Propagation Graph Node:

class TpGraphNode
{
public:
    TpGraphNode(Cm::Sym::VariableSymbol* variableSymbol_, uint32_t variableSymbolSid_);
    uint32_t VariableSymbolSid() const { return variableSymbolSid; }
    const std::string& VariableSymbolFullName() const { return variableSymbolFullName; }
    std::string ReachingClassSetStr() const;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*>& ReachingClasses() { return reachingClasses; }
    void AddTarget(TpGraphNode* target);
    const std::unordered_set<TpGraphNode*>& Targets() const { return targets; }
    bool Visited() const { return visited; }
    void Print(Cm::Util::CodeFormatter& formatter);
    void PrintReachingSet(Cm::Util::CodeFormatter& formatter);
    void Propagate(std::queue<TpGraphNode*>& workList);
private:
    uint32_t variableSymbolSid;
    std::string variableSymbolFullName;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> initClasses;
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> reachingClasses;
    std::unordered_set<TpGraphNode*> targets;
    bool visited;
};

// Type Propagation Graph:

class TpGraph
{
public:
    TpGraph(Cm::Sym::SymbolTable& symbolTable_);
    TpGraphNode* GetNode(Cm::Sym::VariableSymbol* variableSymbol);
    void Print(const std::string& dotFileName);
    void AddRoot(TpGraphNode* root);
    void Process();
    std::unordered_map<uint32_t, TpGraphNode*>& VirtualCallMap() { return virtualCallMap; }
    void IncDevirtualizedFunctionCalls() { ++devirtualizedFunctionCalls; }
    int DevirtualizedFunctionCalls() const { return devirtualizedFunctionCalls; }
private:
    Cm::Sym::SymbolTable& symbolTable;
    std::vector<std::unique_ptr<TpGraphNode>> nodes;
    std::unordered_map<uint32_t, TpGraphNode*> nodeMap;
    std::unordered_set<TpGraphNode*> roots;
    std::unordered_map<uint32_t, TpGraphNode*> virtualCallMap;
    int devirtualizedFunctionCalls;
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
    void Visit(Cm::BoundTree::BoundConversion& boundConversion) override;
    void Visit(Cm::BoundTree::BoundCast& boundCast) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall) override;
    TpGraph& Graph() { return graph; }
    void PrintVirtualCalls();
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
    bool inThrow;
};

} } // namespace Cm::Opt

#endif // CM_OPT_TYPE_PROPAGATION_GRAPH_INCLUDED
