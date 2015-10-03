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

TpGraphNode::TpGraphNode(Cm::Sym::VariableSymbol* variableSymbol_) : variableSymbolSid(variableSymbol_->Sid()), variableSymbolFullName(variableSymbol_->FullName()), visited(false)
{
}

void TpGraphNode::AddTarget(TpGraphNode* target)
{
    targets.insert(target);
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
    formatter.WriteLine(variableSymbolFullName + " : {" + reaching + "}");
    formatter.IncIndent();
    for (TpGraphNode* target : targets)
    {
        formatter.WriteLine(target->variableSymbolFullName);
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

TpGraphNode* TpGraph::GetNode(Cm::Sym::VariableSymbol* variableSymbol)
{
    std::unordered_map<uint32_t, TpGraphNode*>::const_iterator i = nodeMap.find(variableSymbol->Sid());
    if (i != nodeMap.end())
    {
        return i->second;
    }
    TpGraphNode* node = new TpGraphNode(variableSymbol);
    nodes.push_back(std::unique_ptr<TpGraphNode>(node));
    nodeMap[node->VariableSymbolSid()] = node;
    return node;
}

void TpGraph::Print(Cm::Util::CodeFormatter& formatter)
{
    for (const std::unique_ptr<TpGraphNode>& node : nodes)
    {
        if (!node->ReachingClasses().empty())
        {
            node->Print(formatter);
        }
    }
}

void TpGraph::AddRoot(TpGraphNode* root)
{
    roots.insert(root);
}

void TpGraph::Process()
{
    std::queue<TpGraphNode*> workList;
    for (TpGraphNode* root : roots)
    {
        workList.push(root);
    }
    while (!workList.empty())
    {
        TpGraphNode* node = workList.front();
        workList.pop();
        node->Propagate(workList);
    }
}

TpGraphBuilderVisitor::TpGraphBuilderVisitor(TpGraph& graph_) : Cm::BoundTree::Visitor(true), graph(graph_), currentClass(nullptr), inMain(false), sourceNode(nullptr), entryNode(nullptr)
{
}

void TpGraphBuilderVisitor::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    currentClass = &boundClass;
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
    inMain = boundFunction.IsUserMain();
    Cm::Sym::FunctionSymbol* fun = boundFunction.GetFunctionSymbol();
    if (!fun->IsDestructor())
    {
        if (fun->Entry())
        {
            entryNode = graph.GetNode(fun->Entry());
            if (inMain)
            {
                graph.AddRoot(entryNode);
            }
        }
        if (fun->IsStaticConstructor())
        {
            if (currentClass)
            {
                for (Cm::Sym::VariableSymbol* staticMemberVarSymbol : currentClass->Symbol()->StaticMemberVariables())
                {
                    if (entryNode)
                    {
                        TpGraphNode* staticMemberVarNode = graph.GetNode(staticMemberVarSymbol);
                        entryNode->AddTarget(staticMemberVarNode);
                    }
                }
            }
        }
        else if (fun->IsStatic())
        {
            if (currentClass)
            {
                if (currentClass->Symbol()->StaticConstructor())
                {
                    Cm::Sym::FunctionSymbol* staticConstructor = currentClass->Symbol()->StaticConstructor();
                    if (staticConstructor->Entry())
                    {
                        TpGraphNode* staticConstructorEntryNode = graph.GetNode(staticConstructor->Entry());
                        if (entryNode)
                        {
                            entryNode->AddTarget(staticConstructorEntryNode);
                        }
                    }
                }
            }
        }
        currentFunction = fun->FullName();
        for (Cm::Sym::LocalVariableSymbol* localVariableSymbol : boundFunction.LocalVariables())
        {
            if (localVariableSymbol->GetType()->GetBaseType()->IsClassTypeSymbol())
            {
                TpGraphNode* node = graph.GetNode(localVariableSymbol);
                if (inMain)
                {
                    graph.AddRoot(node);
                }
            }
        }
        SetVisitFunctionBody(true);
    }
    else
    {
        SetVisitFunctionBody(false);
    }
}

void TpGraphBuilderVisitor::EndVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    inMain = false;
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement)
{
    Cm::Sym::LocalVariableSymbol* localVariable = boundConstructionStatement.LocalVariable();
    if (localVariable->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        TpGraphNode* node = graph.GetNode(localVariable);
        Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(localVariable->GetType()->GetBaseType());
        PushSourceNode(node);
        AddEdgesFromArgumentsToParameters(boundConstructionStatement.Constructor(), boundConstructionStatement.Arguments());
        PopSourceNode();
        TpGraphNode* thisParamNode = graph.GetNode(boundConstructionStatement.Constructor()->ThisParameter());
        thisParamNode->AddTarget(node);
    }
}

void TpGraphBuilderVisitor::Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement)
{
    Cm::BoundTree::BoundExpression* left = boundAssignmentStatement.Left();
    if (left->GetType() && left->GetType()->GetBaseType()->IsClassTypeSymbol())
    {
        Cm::BoundTree::BoundExpression* right = boundAssignmentStatement.Right();
        if (right->GetType() && right->GetType()->IsPointerToClassType())
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
    void Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp) override;
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

void EdgeAdderVisitor::Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp)
{
    if (boundUnaryOp.GetFunction()->GroupName() == "operator->")
    {
        TpGraphNode* returnValueNode = parent.Graph().GetNode(boundUnaryOp.GetFunction()->ReturnValue());
        returnValueNode->AddTarget(target);
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
        if (parameter->GetType()->GetBaseType()->IsClassTypeSymbol())
        {
            Cm::Sym::ClassTypeSymbol* parameterClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(parameter->GetType()->GetBaseType());
            PushSourceNode(parameterNode);
        }
        else
        {
            PushSourceNode(nullptr);
        }
        const std::unique_ptr<Cm::BoundTree::BoundExpression>& argument = arguments[i];
        EdgeAdderVisitor edgeAdder(*this, parameterNode);
        argument->Accept(edgeAdder);
        PopSourceNode();
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
            thisArgNode->AddTarget(thatArgNode);
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
    if (entryNode && fun->Entry())
    {
        TpGraphNode* funEntry = graph.GetNode(fun->Entry());
        entryNode->AddTarget(funEntry);
    }
    if (inMain)
    {
        int n = int(fun->Parameters().size());
        int start = 0;
        if (fun->IsMemberFunctionSymbol() && !fun->IsStatic())
        {
            ++start;
        }
        for (int i = start; i < n; ++i)
        {
            Cm::Sym::ParameterSymbol* parameterSymbol = fun->Parameters()[i];
            if (parameterSymbol->GetType()->GetBaseType()->IsClassTypeSymbol())
            {
                TpGraphNode* node = graph.GetNode(parameterSymbol);
                graph.AddRoot(node);
            }
        }
    }
    if (boundFunctionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::newCall) && sourceNode)
    {
        if (fun->IsConstructor())
        {
            Cm::Sym::ParameterSymbol* thisParam = fun->ThisParameter();
            Cm::Sym::TypeSymbol* type = thisParam->GetType()->GetBaseType();
            if (type->IsClassTypeSymbol())
            {
                Cm::Sym::ClassTypeSymbol* classTypeSymbol = static_cast<Cm::Sym::ClassTypeSymbol*>(type);
                if (classTypeSymbol->IsVirtual())
                {
                    sourceNode->ReachingClasses().insert(classTypeSymbol);
                }
            }
        }
    }
    AddEdgesFromArgumentsToParameters(fun, boundFunctionCall.Arguments());
    if (boundFunctionCall.GetFlag(Cm::BoundTree::BoundNodeFlags::genVirtualCall))
    {
        std::string s = currentFunction;
        s.append("$").append(boundFunctionCall.GetFunction()->FullName());
        virtualCalls.push_back(std::make_pair(s, graph.GetNode(boundFunctionCall.GetFunction()->ThisParameter())));
    }
}

void TpGraphBuilderVisitor::PushSourceNode(TpGraphNode* sourceNode_)
{
    sourceNodeStack.push(sourceNode);
    sourceNode = sourceNode_;
}

void TpGraphBuilderVisitor::PopSourceNode()
{
    sourceNode = sourceNodeStack.top();
    sourceNodeStack.pop();
}

void TpGraphBuilderVisitor::PrintVirtualCalls(Cm::Util::CodeFormatter& formatter)
{
    formatter.WriteLine("virtual calls:");
    for (const std::pair<std::string, TpGraphNode*>& call : virtualCalls)
    {
        formatter.Write(call.first);
        formatter.Write(": ");
        TpGraphNode* node = call.second;
        node->PrintReachingSet(formatter);
    }
}

} } // namespace Cm::Opt
