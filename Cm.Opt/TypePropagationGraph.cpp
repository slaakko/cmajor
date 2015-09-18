/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Opt/TypePropagationGraph.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <iostream>

namespace Cm { namespace Opt {

class NodeResolverVisitor : public Cm::BoundTree::Visitor
{
public:
    NodeResolverVisitor(TpGraph& graph_);
    void Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable) override;
    void Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable) override;
    void Visit(Cm::BoundTree::BoundParameter& boundParameter) override;
    void Visit(Cm::BoundTree::BoundReturnValue& boundReturnValue) override;
    void Visit(Cm::BoundTree::BoundConversion& boundConversion) override;
    void Visit(Cm::BoundTree::BoundCast& boundCast) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall) override;
    TpGraphNode* GetNode() const { return node; }
private:
    TpGraph& graph;
    TpGraphNode* node;
};

NodeResolverVisitor::NodeResolverVisitor(TpGraph& graph_) : Cm::BoundTree::Visitor(false), graph(graph_), node(nullptr)
{
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    if (boundLocalVariable.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        node = graph.GetNode(boundLocalVariable.Symbol());
    }
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable)
{
    if (boundMemberVariable.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        node = graph.GetNode(boundMemberVariable.Symbol());
    }
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundParameter& boundParameter)
{
    if (boundParameter.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        node = graph.GetNode(boundParameter.Symbol());
    }
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundReturnValue& boundReturnValue)
{
    if (boundReturnValue.GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        node = graph.GetNode(boundReturnValue.Symbol());
    }
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundConversion& boundConversion)
{
    boundConversion.Operand()->Accept(*this);
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundCast& boundCast)
{
    boundCast.Operand()->Accept(*this);
}

void NodeResolverVisitor::Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall)
{
    if (boundFunctionCall.GetFunction()->IsConstructor())
    {
        if (boundFunctionCall.GetFunction()->ThisParameter()->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            node = graph.GetNode(boundFunctionCall.GetFunction()->ThisParameter());
        }
    }
    else if (boundFunctionCall.GetFunction()->GetReturnType())
    {
        if (boundFunctionCall.GetFunction()->GetReturnType()->GetBaseType()->IsClassTypeSymbol())
        {
            node = graph.GetNode(boundFunctionCall.GetFunction()->ReturnValue());
        }
    }
}

TpGraphNode::TpGraphNode(Cm::Sym::VariableSymbol* variableSymbol_) : variableSymbol(variableSymbol_), isTarget(false), visited(false)
{
}

void TpGraphNode::AddTarget(TpGraphNode* target)
{
    targets.insert(target);
    target->SetTarget();
}

void TpGraphNode::Print(Cm::Util::CodeFormatter& formatter)
{
    std::string reaching;
    bool first = true;
    for (Cm::Sym::ClassTypeSymbol* r : reachingClasses)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            reaching.append(", ");
        }
        reaching.append(r->FullName());
    }
    formatter.WriteLine(variableSymbol->FullName() + " : {" + reaching + "}");
    formatter.IncIndent();
    for (TpGraphNode* target : targets)
    {
        formatter.WriteLine(target->variableSymbol->FullName());
    }
    formatter.DecIndent();
}

void TpGraphNode::PrintReachingSet(Cm::Util::CodeFormatter& formatter)
{
    std::string reaching;
    bool first = true;
    for (Cm::Sym::ClassTypeSymbol* r : reachingClasses)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            reaching.append(", ");
        }
        reaching.append(r->FullName());
    }
    formatter.WriteLine("{" + reaching + "}");
}

void TpGraphNode::Propagate(std::queue<TpGraphNode*>& workList)
{
    visited = true;
    for (TpGraphNode* target : targets)
    {
        int n = int(target->ReachingClasses().size());
        target->ReachingClasses().insert(reachingClasses.begin(), reachingClasses.end());
        if (!target->visited || n != target->ReachingClasses().size())
        {
            workList.push(target);
        }
    }
}

TpGraphNode* TpGraph::GetNode(Cm::Sym::VariableSymbol* symbol)
{
    std::unordered_map<Cm::Sym::VariableSymbol*, TpGraphNode*>::const_iterator i = nodeMap.find(symbol);
    if (i != nodeMap.end())
    {
        return i->second;
    }
    TpGraphNode* node = new TpGraphNode(symbol);
    nodes.push_back(std::unique_ptr<TpGraphNode>(node));
    nodeMap[node->Symbol()] = node;
    return node;
}

void TpGraph::Print(Cm::Util::CodeFormatter& formatter)
{
    for (const std::unique_ptr<TpGraphNode>& node : nodes)
    {
        node->Print(formatter);
    }
}

void TpGraph::Process()
{
    std::queue<TpGraphNode*> workList;
    for (const std::unique_ptr<TpGraphNode>& node : nodes)
    {
        if (!node->IsTarget())
        {
            workList.push(node.get());
        }
    }
    while (!workList.empty())
    {
        TpGraphNode* node = workList.front();
        workList.pop();
        node->Propagate(workList);
    }
}

TpGraphBuilderVisitor::TpGraphBuilderVisitor(TpGraph& graph_) : Cm::BoundTree::Visitor(true), graph(graph_)
{
}

void TpGraphBuilderVisitor::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    Cm::Sym::ClassTypeSymbol* cls = boundClass.Symbol();
    for (Cm::Sym::MemberVariableSymbol* memberVariableSymbol : cls->MemberVariables())
    {
        if (memberVariableSymbol->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            graph.GetNode(memberVariableSymbol);
        }
    }
}

void TpGraphBuilderVisitor::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    Cm::Sym::FunctionSymbol* fun = boundFunction.GetFunctionSymbol();
    if (!fun->IsDestructor())
    {
        SetVisitFunctionBody(true);
        bool first = true;
        for (Cm::Sym::ParameterSymbol* parameterSymbol : fun->Parameters())
        {
            if (parameterSymbol->GetType()->GetBaseType()->IsClassTypeSymbol())
            {
                TpGraphNode* node = graph.GetNode(parameterSymbol);
                if (first)
                {
                    first = false;
                    if (fun->IsConstructor() && parameterSymbol->GetType()->GetPointerCount() == 1)
                    {
                        Cm::Sym::ClassTypeSymbol* classType = static_cast<Cm::Sym::ClassTypeSymbol*>(parameterSymbol->GetType()->GetBaseType());
                        node->ReachingClasses().insert(classType);
                    }
                }
            }
        }
        if (fun->ReturnValue())
        {
            if (fun->ReturnValue()->GetType()->GetBaseType()->IsClassTypeSymbol())
            {
                graph.GetNode(fun->ReturnValue());
            }
        }
    }
    else
    {
        SetVisitFunctionBody(false);
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    Cm::Sym::LocalVariableSymbol* localVariable = boundConstructionStatement.LocalVariable();
    if (localVariable->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* node = graph.GetNode(localVariable);
        AddEdgesFromArgumentsToParameters(boundConstructionStatement.Constructor(), boundConstructionStatement.Arguments());
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    Cm::BoundTree::BoundExpression* left = boundAssignmentStatement.Left();
    if (left->GetType() && left->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        Cm::BoundTree::BoundExpression* right = boundAssignmentStatement.Right();
        if (right->GetType() && right->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            Cm::BoundTree::BoundExpressionList arguments;
            arguments.Add(left);
            arguments.Add(right);
            AddEdgesFromArgumentsToParameters(boundAssignmentStatement.Assignment(), arguments);
            arguments.GetLast();
            arguments.GetLast();
        }
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement)
{
    Cm::Sym::MemberVariableSymbol* memberVariable = boundInitMemberVariableStatement.GetMemberVariableSymbol();
    if (memberVariable->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* node = graph.GetNode(memberVariable);
        AddEdgesFromArgumentsToParameters(boundInitMemberVariableStatement.Constructor(), boundInitMemberVariableStatement.Arguments());
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement)
{
    if (boundReturnStatement.ReturnsValue())
    {
        Cm::Sym::VariableSymbol* returnValue = boundReturnStatement.ReturnValue()->Symbol();
        if (returnValue->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            TpGraphNode* target = graph.GetNode(returnValue);
            if (boundReturnStatement.Constructor()->ThisParameter()->GetType()->GetBaseType()->IsClassTypeSymbol())
            {
                NodeResolverVisitor resolver(graph);
                boundReturnStatement.Expression()->Accept(resolver);
                TpGraphNode* source = resolver.GetNode();
                if (source)
                {
                    source->AddTarget(target);
                }
            }
        }
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement)
{
    if (boundSimpleStatement.GetExpression())
    {
        boundSimpleStatement.GetExpression()->Accept(*this);
    }
}

class EdgeAdderVisitor : public Cm::BoundTree::Visitor
{
public:
    EdgeAdderVisitor(TpGraphBuilderVisitor& parent_, TpGraphNode* target_);
    void Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable) override;
    void Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable) override;
    void Visit(Cm::BoundTree::BoundParameter& boundParameter) override;
    void Visit(Cm::BoundTree::BoundReturnValue& boundReturnValue) override;
    void Visit(Cm::BoundTree::BoundConversion& boundConversion) override;
    void Visit(Cm::BoundTree::BoundCast& boundCast) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall) override;
private:
    TpGraphBuilderVisitor& parent;
    TpGraphNode* target;
};

EdgeAdderVisitor::EdgeAdderVisitor(TpGraphBuilderVisitor& parent_, TpGraphNode* target_) : Cm::BoundTree::Visitor(false), parent(parent_), target(target_)
{
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable)
{
    if (boundLocalVariable.Symbol() && boundLocalVariable.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* localVariableNode = parent.Graph().GetNode(boundLocalVariable.Symbol());
        localVariableNode->AddTarget(target);
    }
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable)
{
    if (boundMemberVariable.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* memberVariableNode = parent.Graph().GetNode(boundMemberVariable.Symbol());
        memberVariableNode->AddTarget(target);
    }
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundParameter& boundParameter)
{
    if (boundParameter.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* parameterNode = parent.Graph().GetNode(boundParameter.Symbol());
        parameterNode->AddTarget(target);
    }
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundReturnValue& boundReturnValue)
{
    if (boundReturnValue.Symbol()->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* returnValueNode = parent.Graph().GetNode(boundReturnValue.Symbol());
        returnValueNode->AddTarget(target);
    }
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundConversion& boundConversion)
{
    boundConversion.Operand()->Accept(*this);
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundCast& boundCast)
{
    boundCast.Operand()->Accept(*this);
}

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall)
{
    parent.Visit(boundFunctionCall);
    if (boundFunctionCall.GetFunction()->IsConstructor())
    {
        if (boundFunctionCall.GetFunction()->ThisParameter()->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            TpGraphNode* thisParameterNode = parent.Graph().GetNode(boundFunctionCall.GetFunction()->ThisParameter());
            thisParameterNode->AddTarget(target);
        }
    }
    else if (boundFunctionCall.GetFunction()->GetReturnType())
    {
        if (boundFunctionCall.GetFunction()->GetReturnType()->GetBaseType()->IsClassTypeSymbol())
        {
            TpGraphNode* returnValueNode = parent.Graph().GetNode(boundFunctionCall.GetFunction()->ReturnValue());
            returnValueNode->AddTarget(target);
        }
    }
}

void TpGraphBuilderVisitor::AddEdgesFromArgumentsToParameters(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::BoundExpressionList& arguments)
{
    int n = int(fun->Parameters().size());
    if (arguments.Count() != n)
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* parameter = fun->Parameters()[i];
        TpGraphNode* parameterNode = graph.GetNode(parameter);
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
        EdgeAdderVisitor edgeAdder(*this, parameterNode);
        argument->Accept(edgeAdder);
    }
    if (fun->IsBasicTypeCopyMoveOrAssignmentOp() || fun->IsCopyConstructor() || fun->IsCopyAssignment() || fun->IsMoveConstructor() || fun->IsMoveAssignment() || fun->IsConvertingConstructor())
    {
        NodeResolverVisitor thatNodeResolver(graph);
        arguments[1]->Accept(thatNodeResolver);
        TpGraphNode* thatArgNode = thatNodeResolver.GetNode();
        NodeResolverVisitor thisNodeResolver(graph);
        arguments[0]->Accept(thisNodeResolver);
        TpGraphNode* thisArgNode = thisNodeResolver.GetNode();
        if (thisArgNode && thatArgNode)
        {
            thatArgNode->AddTarget(thisArgNode);
        }
    }
    for (Cm::Sym::FunctionSymbol* overrider : fun->OverrideSet())
    {
        AddEdgesFromArgumentsToParameters(overrider, arguments);
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundFunctionCall& boundFunctionCall)
{
    Cm::Sym::FunctionSymbol* fun = boundFunctionCall.GetFunction();
    AddEdgesFromArgumentsToParameters(fun, boundFunctionCall.Arguments());
    if (boundFunctionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall))
    {
        virtualCalls.insert(&boundFunctionCall);
    }
}

void TpGraphBuilderVisitor::PrintVirtualCalls(Cm::Util::CodeFormatter& formatter)
{
    formatter.WriteLine("virtual calls:");
    for (Cm::BoundTree::BoundFunctionCall* call : virtualCalls)
    {
        formatter.Write(call->GetFunction()->FullName());
        formatter.Write(": ");
        TpGraphNode* node = graph.GetNode(call->GetFunction()->ThisParameter());
        node->PrintReachingSet(formatter);
    }
}

} } // namespace Cm::Opt
