/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <stdexcept>

namespace Cm { namespace BoundTree {

BoundStatement::BoundStatement(Cm::Ast::Node* syntaxNode_) : BoundNode(syntaxNode_), parent(nullptr), cfgNode(nullptr), statementId(noStatementdId)
{
    if (syntaxNode_)
    {
        if (syntaxNode_->IsStatementNode())
        {
            Cm::Ast::StatementNode* statementNode = static_cast<Cm::Ast::StatementNode*>(syntaxNode_);
            Cm::Ast::LabelNode* labelNode = statementNode->Label();
            if (labelNode)
            {
                label = labelNode->Label();
            }
        }
    }
}

void BoundStatement::SetStatementId(uint32_t statementId_)
{
    statementId = statementId_;
}

void BoundStatement::SetLabeledStatementId(uint32_t labeledStatementId_)
{
    labeledStatementId = labeledStatementId_;
}

void BoundStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundNode::Write(writer);
    if (statementId == noStatementdId)
    {
        statementId = writer.GetStatementId();
    }
    writer.GetBinaryWriter().Write(statementId);
    bool hasLabel = !label.empty();
    writer.GetBinaryWriter().Write(hasLabel);
    if (hasLabel)
    {
        writer.GetBinaryWriter().Write(label);
        if (labeledStatementId == noStatementdId)
        {
            labeledStatementId = writer.GetStatementId();
        }
        writer.GetBinaryWriter().Write(labeledStatementId);
    }
}

void BoundStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundNode::Read(reader);
    statementId = reader.GetBinaryReader().ReadUInt();
    reader.SetCompoundTargetStatement(statementId, this);
    bool hasLabel = reader.GetBinaryReader().ReadBool();
    if (hasLabel)
    {
        label = reader.GetBinaryReader().ReadString();
        labeledStatementId = reader.GetBinaryReader().ReadUInt();
        reader.SetLabeledStatement(labeledStatementId, this);
    }
}

BoundCompoundStatement* BoundStatement::CompoundParent() const
{
    BoundStatement* p = parent;
    while (p && !p->IsBoundCompoundStatement())
    {
        p = p->parent;
    }
    if (!p)
    {
        return nullptr;
    }
    return static_cast<BoundCompoundStatement*>(p);
}

void BoundStatement::AddToPrevSet(Cm::Core::CfgNode* node)
{
    prevSet.insert(node);
}

void BoundStatement::SetCfgNode(Cm::Core::CfgNode* cfgNode_)
{ 
    cfgNode = cfgNode_; 
    PatchPrevSet();
}

void BoundStatement::PatchPrevSet()
{
    if (!cfgNode)
    {
        throw std::runtime_error("cfg node not set");
    }
    int thisNodeId = cfgNode->Id();
    for (Cm::Core::CfgNode* prev : prevSet)
    {
        prev->AddNext(thisNodeId);
    }
    prevSet.clear();
}

BoundStatementList::BoundStatementList()
{
}

void BoundStatementList::Write(Cm::Sym::BcuWriter& writer)
{
    int n = int(statements.size());
    writer.GetBinaryWriter().Write(n);
    for (const std::unique_ptr<BoundStatement>& statement : statements)
    {
        writer.Write(statement.get());
    }
}

void BoundStatementList::Read(Cm::Sym::BcuReader& reader)
{
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::BcuItem* item = reader.ReadItem();
        if (item->IsBoundStatement())
        {
            statements.push_back(std::unique_ptr<BoundStatement>(static_cast<BoundStatement*>(item)));
        }
        else
        {
            throw std::runtime_error("statement expected");
        }
    }
}

void BoundStatementList::AddStatement(BoundStatement* statement)
{
    statements.push_back(std::unique_ptr<BoundStatement>(statement));
}

void BoundStatementList::InsertStatement(int index, BoundStatement* statement)
{
    statements.insert(statements.begin() + index, std::unique_ptr<BoundStatement>(statement));
}

void BoundStatementList::Accept(Visitor& visitor)
{
    for (const std::unique_ptr<BoundStatement>& statement : statements)
    {
        visitor.VisitStatement(*statement);
    }
}

BoundParentStatement::BoundParentStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

BoundCompoundStatement::BoundCompoundStatement() : BoundParentStatement(nullptr)
{
}

BoundCompoundStatement::BoundCompoundStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundCompoundStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    statementList.Write(writer);
}

void BoundCompoundStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    statementList.Read(reader);
}

void BoundCompoundStatement::AddStatement(BoundStatement* statement)
{
    statement->SetParent(this);
    statementList.AddStatement(statement);
}

void BoundCompoundStatement::InsertStatement(int index, BoundStatement* statement)
{
    statement->SetParent(this);
    statementList.InsertStatement(index, statement);
}

void BoundCompoundStatement::Accept(Visitor& visitor) 
{
    visitor.BeginVisit(*this);
    statementList.Accept(visitor);
    visitor.EndVisit(*this);
}

BoundReceiveStatement::BoundReceiveStatement() : BoundStatement(nullptr), parameterSymbol(nullptr), ctor(nullptr)
{
}

BoundReceiveStatement::BoundReceiveStatement(Cm::Sym::ParameterSymbol* parameterSymbol_) : BoundStatement(nullptr), parameterSymbol(parameterSymbol_), ctor(nullptr)
{
}

void BoundReceiveStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(parameterSymbol);
    writer.Write(ctor);
}

void BoundReceiveStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::Symbol* p = reader.ReadSymbol();
    if (p->IsParameterSymbol())
    {
        parameterSymbol = static_cast<Cm::Sym::ParameterSymbol*>(p);
    }
    else
    {
        throw std::runtime_error("parameter symbol expected");
    }
    Cm::Sym::Symbol* c = reader.ReadSymbol();
    if (c->IsFunctionSymbol())
    {
        ctor = static_cast<Cm::Sym::FunctionSymbol*>(c);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
}

void BoundReceiveStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitClassObjectStatement::BoundInitClassObjectStatement() : BoundStatement(nullptr), functionCall(nullptr)
{
}

BoundInitClassObjectStatement::BoundInitClassObjectStatement(BoundFunctionCall* functionCall_) : BoundStatement(nullptr), functionCall(functionCall_)
{
}

void BoundInitClassObjectStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(functionCall.get());
}

void BoundInitClassObjectStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item->IsBoundFunctionCall())
    {
        functionCall.reset(static_cast<BoundFunctionCall*>(item));
    }
    else
    {
        throw std::runtime_error("bound function call expected");
    }
}

void BoundInitClassObjectStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitVPtrStatement::BoundInitVPtrStatement() : BoundStatement(nullptr), classType(nullptr)
{
}

BoundInitVPtrStatement::BoundInitVPtrStatement(Cm::Sym::ClassTypeSymbol* classType_) : BoundStatement(nullptr), classType(classType_)
{
}

void BoundInitVPtrStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(classType);
}

void BoundInitVPtrStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    classType = reader.ReadClassTypeSymbol();
}

void BoundInitVPtrStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundInitMemberVariableStatement::BoundInitMemberVariableStatement() : BoundStatement(nullptr), ctor(nullptr), arguments(), registerDestructor(false), memberVarSymbol(nullptr)
{
}

BoundInitMemberVariableStatement::BoundInitMemberVariableStatement(Cm::Sym::FunctionSymbol* ctor_, BoundExpressionList&& arguments_) : 
    BoundStatement(nullptr), ctor(ctor_), arguments(std::move(arguments_)), registerDestructor(false), memberVarSymbol(nullptr)
{
}

void BoundInitMemberVariableStatement::Write(Cm::Sym::BcuWriter& writer)
{
    if (memberVarSymbol->Name() == "mt")
    {
        int x = 0;
    }
    BoundStatement::Write(writer);
    writer.Write(memberVarSymbol);
    writer.Write(ctor);
    arguments.Write(writer);
    writer.GetBinaryWriter().Write(registerDestructor);
}

void BoundInitMemberVariableStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::Symbol* s = reader.ReadSymbol();
    if (s->IsMemberVariableSymbol())
    {
        memberVarSymbol = static_cast<Cm::Sym::MemberVariableSymbol*>(s);
    }
    else
    {
        throw std::runtime_error("member variable symbol expected");
    }
    Cm::Sym::Symbol* f = reader.ReadSymbol();
    if (f->IsFunctionSymbol())
    {
        ctor = static_cast<Cm::Sym::FunctionSymbol*>(f);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    arguments.Read(reader);
    registerDestructor = reader.GetBinaryReader().ReadBool();
}

void BoundInitMemberVariableStatement::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

void BoundInitMemberVariableStatement::SetMemberVariableSymbol(Cm::Sym::MemberVariableSymbol* memberVarSymbol_)
{
    memberVarSymbol = memberVarSymbol_;
}

BoundFunctionCallStatement::BoundFunctionCallStatement() : BoundStatement(nullptr), function(nullptr), arguments()
{
}

BoundFunctionCallStatement::BoundFunctionCallStatement(Cm::Sym::FunctionSymbol* function_, BoundExpressionList&& arguments_) : BoundStatement(nullptr), function(function_), arguments(std::move(arguments_))
{
}

void BoundFunctionCallStatement::Write(Cm::Sym::BcuWriter& writer) 
{
    BoundStatement::Write(writer);
    writer.Write(function);
    arguments.Write(writer);
    writer.Write(traceCallInfo.get());
}

void BoundFunctionCallStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::Symbol* f = reader.ReadSymbol();
    if (f->IsFunctionSymbol())
    {
        function = static_cast<Cm::Sym::FunctionSymbol*>(f);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    arguments.Read(reader);
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
}

void BoundFunctionCallStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundFunctionCallStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundReturnStatement::BoundReturnStatement() : BoundStatement(nullptr), ctor(nullptr), returnType(nullptr)
{
}

BoundReturnStatement::BoundReturnStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_), ctor(nullptr), returnType(nullptr)
{
}

void BoundReturnStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(expression.get());
    writer.Write(ctor);
    writer.Write(returnType);
    writer.Write(traceCallInfo.get());
    writer.Write(boundTemporary.get());
    writer.Write(boundReturnValue.get());
}

void BoundReturnStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::BcuItem* exprItem = reader.ReadItem();
    if (exprItem)
    {
        if (exprItem->IsBoundExpression())
        {
            expression.reset(static_cast<BoundExpression*>(exprItem));
        }
        else
        {
            throw std::runtime_error("bound expression expected");
        }
    }
    Cm::Sym::Symbol* c = reader.ReadSymbol();
    if (c)
    {
        if (c->IsFunctionSymbol())
        {
            ctor = static_cast<Cm::Sym::FunctionSymbol*>(c);
        }
        else
        {
            throw std::runtime_error("function symbol expected");
        }
    }
    Cm::Sym::Symbol* t = reader.ReadSymbol();
    if (t)
    {
        if (t->IsTypeSymbol())
        {
            returnType = static_cast<Cm::Sym::TypeSymbol*>(t);
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
    Cm::Sym::BcuItem* temp = reader.ReadItem();
    if (temp)
    {
        if (temp->IsBoundLocalVariable())
        {
            boundTemporary.reset(static_cast<BoundLocalVariable*>(temp));
        }
        else
        {
            throw std::runtime_error("bound local variable expected");
        }
    }
    Cm::Sym::BcuItem* retVal = reader.ReadItem();
    if (retVal)
    {
        if (retVal->IsBoundReturnValue())
        {
            boundReturnValue.reset(static_cast<BoundReturnValue*>(retVal));
        }
        else
        {
            throw std::runtime_error("bound return value expected");
        }
    }
}

void BoundReturnStatement::SetExpression(BoundExpression* expression_)
{
    expression.reset(expression_);
}

void BoundReturnStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundReturnStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

void BoundReturnStatement::SetBoundTemporary(Cm::BoundTree::BoundLocalVariable* boundTemporary_)
{
    boundTemporary.reset(boundTemporary_);
}

void BoundReturnStatement::SetBoundReturnValue(BoundReturnValue* returnValue)
{
    boundReturnValue.reset(returnValue);
}

BoundBeginTryStatement::BoundBeginTryStatement() : BoundStatement(nullptr), firstCatchId(-1)
{
}

BoundBeginTryStatement::BoundBeginTryStatement(Cm::Ast::Node* syntaxNode_, int firstCatchId_) : BoundStatement(syntaxNode_), firstCatchId(firstCatchId_)
{
}

void BoundBeginTryStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.GetBinaryWriter().Write(firstCatchId);
}

void BoundBeginTryStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    firstCatchId = reader.GetBinaryReader().ReadInt();
}

void BoundBeginTryStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundEndTryStatement::BoundEndTryStatement() : BoundStatement(nullptr)
{
}

BoundEndTryStatement::BoundEndTryStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundEndTryStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundExitBlocksStatement::BoundExitBlocksStatement() : BoundStatement(nullptr), targetBlock(nullptr)
{
}

BoundExitBlocksStatement::BoundExitBlocksStatement(Cm::Ast::Node* syntaxNode_, BoundCompoundStatement* targetBlock_) : BoundStatement(syntaxNode_), targetBlock(targetBlock_)
{
}

void BoundExitBlocksStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    if (targetBlock->GetStatementId() == noStatementdId)
    {
        targetBlock->SetStatementId(writer.GetStatementId());
    }
    writer.GetBinaryWriter().Write(targetBlock->GetStatementId());
}

void BoundExitBlocksStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    uint32_t statementId = reader.GetBinaryReader().ReadUInt();
    reader.FetchCompoundTargetStatement(this, statementId);
}

void BoundExitBlocksStatement::SetCompoundTargetStatement(void* targetStatement)
{
    targetBlock = reinterpret_cast<BoundCompoundStatement*>(targetStatement);
}

void BoundExitBlocksStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundPushGenDebugInfoStatement::BoundPushGenDebugInfoStatement() : BoundStatement(nullptr), generate(false)
{
}

BoundPushGenDebugInfoStatement::BoundPushGenDebugInfoStatement(Cm::Ast::Node* syntaxNode_, bool generate_) : BoundStatement(syntaxNode_), generate(generate_)
{
}

void BoundPushGenDebugInfoStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.GetBinaryWriter().Write(generate);
}

void BoundPushGenDebugInfoStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    generate = reader.GetBinaryReader().ReadBool();
}

void BoundPushGenDebugInfoStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundPopGenDebugInfoStatement::BoundPopGenDebugInfoStatement() : BoundStatement(nullptr)
{
}

BoundPopGenDebugInfoStatement::BoundPopGenDebugInfoStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundPopGenDebugInfoStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBeginThrowStatement::BoundBeginThrowStatement() : BoundStatement(nullptr)
{
}

BoundBeginThrowStatement::BoundBeginThrowStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBeginThrowStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundEndThrowStatement::BoundEndThrowStatement() : BoundStatement(nullptr)
{
}

BoundEndThrowStatement::BoundEndThrowStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundEndThrowStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBeginCatchStatement::BoundBeginCatchStatement() : BoundStatement(nullptr)
{
}

BoundBeginCatchStatement::BoundBeginCatchStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBeginCatchStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstructionStatement::BoundConstructionStatement() : BoundStatement(nullptr), localVariable(nullptr), ctor(nullptr)
{
}

BoundConstructionStatement::BoundConstructionStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_), localVariable(nullptr), ctor(nullptr)
{
}

void BoundConstructionStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(localVariable);
    arguments.Write(writer);
    writer.Write(ctor);
    writer.Write(traceCallInfo.get());
}

void BoundConstructionStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::Symbol* l = reader.ReadSymbol();
    if (l->IsLocalVariableSymbol())
    {
        localVariable = static_cast<Cm::Sym::LocalVariableSymbol*>(l);
    }
    else
    {
        throw std::runtime_error("local variable symbol expected");
    }
    arguments.Read(reader);
    Cm::Sym::Symbol* f = reader.ReadSymbol();
    if (f->IsFunctionSymbol())
    {
        ctor = static_cast<Cm::Sym::FunctionSymbol*>(f);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
}

void BoundConstructionStatement::SetArguments(BoundExpressionList&& arguments_) 
{ 
    arguments = std::move(arguments_); 
}

void BoundConstructionStatement::InsertLocalVariableToArguments()
{
    Cm::BoundTree::BoundLocalVariable* boundLocalVariable = new Cm::BoundTree::BoundLocalVariable(SyntaxNode(), localVariable);
    boundLocalVariable->SetType(localVariable->GetType());
    arguments.InsertFront(boundLocalVariable);
}

void BoundConstructionStatement::GetResolutionArguments(Cm::Sym::TypeSymbol* localVariableType, std::vector<Cm::Core::Argument>& resolutionArguments)
{
    if (localVariableType->IsPointerType() && arguments.Count() == 1 && arguments[0]->IsBoundNullLiteral())
    {
        arguments[0]->SetType(localVariableType);
    }
    for (const std::unique_ptr<BoundExpression>& argument : arguments)
    {
        Cm::Core::Argument arg(argument->GetArgumentCategory(), argument->GetType());
        if (argument->GetFlag(Cm::BoundTree::BoundNodeFlags::argIsTemporary))
        {
            arg.SetBindToRvalueRef();
        }
        resolutionArguments.push_back(arg);
    }
}

void BoundConstructionStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundConstructionStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundDestructionStatement::BoundDestructionStatement(Cm::Ast::Node* syntaxNode_, Ir::Intf::Object* object_, Cm::Sym::FunctionSymbol* destructor_) :
    BoundStatement(syntaxNode_), object(object_), destructor(destructor_)
{
}

void BoundDestructionStatement::Accept(Visitor& visitor) 
{
    visitor.Visit(*this);
}

BoundAssignmentStatement::BoundAssignmentStatement() : BoundStatement(nullptr), left(nullptr), right(nullptr), assignment(nullptr)
{
}

BoundAssignmentStatement::BoundAssignmentStatement(Cm::Ast::Node* syntaxNode_, BoundExpression* left_, BoundExpression* right_, Cm::Sym::FunctionSymbol* assignment_) : 
    BoundStatement(syntaxNode_), left(left_), right(right_), assignment(assignment_)
{
}

void BoundAssignmentStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(left.get());
    writer.Write(right.get());
    writer.Write(assignment);
    writer.Write(traceCallInfo.get());
}

void BoundAssignmentStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::BcuItem* leftItem = reader.ReadItem();
    if (leftItem->IsBoundExpression())
    {
        left.reset(static_cast<BoundExpression*>(leftItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* rightItem = reader.ReadItem();
    if (rightItem->IsBoundExpression())
    {
        right.reset(static_cast<BoundExpression*>(rightItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::Symbol* a = reader.ReadSymbol();
    if (a->IsFunctionSymbol())
    {
        assignment = static_cast<Cm::Sym::FunctionSymbol*>(a);
    }
    else
    {
        throw std::runtime_error("function symbol expected");
    }
    Cm::Sym::BcuItem* traceCallItem = reader.ReadItem();
    if (traceCallItem)
    {
        if (traceCallItem->IsTraceCall())
        {
            traceCallInfo.reset(static_cast<TraceCallInfo*>(traceCallItem));
        }
        else
        {
            throw std::runtime_error("trace call info expected");
        }
    }
}

void BoundAssignmentStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundAssignmentStatement::SetTraceCallInfo(TraceCallInfo* traceCallInfo_)
{
    traceCallInfo.reset(traceCallInfo_);
}

BoundSimpleStatement::BoundSimpleStatement() : BoundStatement(nullptr)
{
}

BoundSimpleStatement::BoundSimpleStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundSimpleStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.Write(expression.get());
}

void BoundSimpleStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    Cm::Sym::BcuItem* item = reader.ReadItem();
    if (item)
    {
        if (item->IsBoundExpression())
        {
            expression.reset(static_cast<BoundExpression*>(item));
        }
        else
        {
            throw std::runtime_error("bound expression expected");
        }
    }
}

void BoundSimpleStatement::SetExpression(BoundExpression* expression_)
{
    expression.reset(expression_);
}

void BoundSimpleStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundSwitchStatement::BoundSwitchStatement() : BoundParentStatement(nullptr)
{
}

BoundSwitchStatement::BoundSwitchStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundSwitchStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    writer.Write(condition.get());
    caseStatements.Write(writer);
    writer.Write(defaultStatement.get());
}

void BoundSwitchStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    Cm::Sym::BcuItem* condItem = reader.ReadItem();
    if (condItem->IsBoundExpression())
    {
        condition.reset(static_cast<BoundExpression*>(condItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    caseStatements.Read(reader);
    Cm::Sym::BcuItem* s = reader.ReadItem();
    if (s)
    {
        if (s->IsBoundStatement())
        {
            defaultStatement.reset(static_cast<BoundStatement*>(s));
        }
        else
        {
            throw std::runtime_error("bound statement expected");
        }
    }
}

void BoundSwitchStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundSwitchStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    if (statement_->IsBoundCaseStatement())
    {
        caseStatements.AddStatement(statement_);
    }
    else
    {
        defaultStatement.reset(statement_);
    }
}

void BoundSwitchStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundSwitchStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel)
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundSwitchStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

BoundCaseStatement::BoundCaseStatement() : BoundParentStatement(nullptr)
{
}

BoundCaseStatement::BoundCaseStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundCaseStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    statements.Write(writer);
    int n = int(values.size());
    writer.GetBinaryWriter().Write(n);
    for (const std::unique_ptr<Cm::Sym::Value>& value : values)
    {
        writer.GetSymbolWriter().Write(value.get());
    }
}

void BoundCaseStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    statements.Read(reader);
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Cm::Sym::Value> value(reader.GetSymbolReader().ReadValue());
        values.push_back(std::move(value));
    }
}

void BoundCaseStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statements.AddStatement(statement_);
}

void BoundCaseStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoundCaseStatement::AddValue(Cm::Sym::Value* value)
{
    values.push_back(std::unique_ptr<Cm::Sym::Value>(value));
}

BoundDefaultStatement::BoundDefaultStatement() : BoundParentStatement(nullptr)
{
}

BoundDefaultStatement::BoundDefaultStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundDefaultStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    statements.Write(writer);
}

void BoundDefaultStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    statements.Read(reader);
}

void BoundDefaultStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statements.AddStatement(statement_);
}

void BoundDefaultStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundBreakStatement::BoundBreakStatement() : BoundStatement(nullptr)
{
}

BoundBreakStatement::BoundBreakStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundBreakStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundContinueStatement::BoundContinueStatement() : BoundStatement(nullptr)
{
}

BoundContinueStatement::BoundContinueStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundContinueStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoStatement::BoundGotoStatement() : BoundStatement(nullptr), targetLabel(), targetStatement(nullptr), targetCompoundParent(nullptr), isExceptionHandlingGoto(false)
{
}

BoundGotoStatement::BoundGotoStatement(Cm::Ast::Node* syntaxNode_, const std::string& targetLabel_) : 
    BoundStatement(syntaxNode_), targetLabel(targetLabel_), targetStatement(nullptr), targetCompoundParent(nullptr), isExceptionHandlingGoto(false)
{
}

void BoundGotoStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.GetBinaryWriter().Write(targetLabel);
    if (targetStatement->GetLabeledStatementId() == noStatementdId)
    {
        targetStatement->SetLabeledStatementId(writer.GetStatementId());
    }
    writer.GetBinaryWriter().Write(targetStatement->GetLabeledStatementId());
    if (targetCompoundParent->GetStatementId() == noStatementdId)
    {
        targetCompoundParent->SetStatementId(writer.GetStatementId());
    }
    writer.GetBinaryWriter().Write(targetCompoundParent->GetStatementId());
    writer.GetBinaryWriter().Write(isExceptionHandlingGoto);
}

void BoundGotoStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    targetLabel = reader.GetBinaryReader().ReadString();
    uint32_t labeledStatementId = reader.GetBinaryReader().ReadUInt();
    reader.FetchLabeledStatement(this, labeledStatementId);
    uint32_t statementId = reader.GetBinaryReader().ReadUInt();
    reader.FetchCompoundTargetStatement(this, statementId);
    isExceptionHandlingGoto = reader.GetBinaryReader().ReadBool();
}

void BoundGotoStatement::SetLabeledStatement(void* labeledStatement)
{
    targetStatement = reinterpret_cast<BoundStatement*>(labeledStatement);
}

void BoundGotoStatement::SetCompoundTargetStatement(void* targetStatement)
{
    targetCompoundParent = reinterpret_cast<BoundCompoundStatement*>(targetStatement);
}

void BoundGotoStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoCaseStatement::BoundGotoCaseStatement() : BoundStatement(nullptr)
{
}

BoundGotoCaseStatement::BoundGotoCaseStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundGotoCaseStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundStatement::Write(writer);
    writer.GetSymbolWriter().Write(value.get());
}

void BoundGotoCaseStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundStatement::Read(reader);
    value.reset(reader.GetSymbolReader().ReadValue());
}

void BoundGotoCaseStatement::SetValue(Cm::Sym::Value* value_)
{
    value.reset(value_);
}

void BoundGotoCaseStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundGotoDefaultStatement::BoundGotoDefaultStatement() : BoundStatement(nullptr)
{
}

BoundGotoDefaultStatement::BoundGotoDefaultStatement(Cm::Ast::Node* syntaxNode_) : BoundStatement(syntaxNode_)
{
}

void BoundGotoDefaultStatement::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BoundConditionalStatement::BoundConditionalStatement() : BoundParentStatement(nullptr)
{
}

BoundConditionalStatement::BoundConditionalStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundConditionalStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    writer.Write(condition.get());
    writer.Write(thenS.get());
    writer.Write(elseS.get());
}

void BoundConditionalStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    Cm::Sym::BcuItem* condItem = reader.ReadItem();
    if (condItem->IsBoundExpression())
    {
        condition.reset(static_cast<BoundExpression*>(condItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* thenItem = reader.ReadItem();
    if (thenItem->IsBoundStatement())
    {
        thenS.reset(static_cast<BoundStatement*>(thenItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
    Cm::Sym::BcuItem* elseItem = reader.ReadItem();
    if (elseItem)
    {
        if (elseItem->IsBoundStatement())
        {
            elseS.reset(static_cast<BoundStatement*>(elseItem));
        }
        else
        {
            throw std::runtime_error("bound statement expected");
        }
    }
}

void BoundConditionalStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundConditionalStatement::AddStatement(BoundStatement* statement)
{
    statement->SetParent(this);
    if (!thenS)
    {
        thenS.reset(statement);
    }
    else
    {
        elseS.reset(statement);
    }
}

void BoundConditionalStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    condition->Accept(visitor);
    if (!visitor.SkipContent())
    {
        thenS->Accept(visitor);
        if (elseS)
        {
            elseS->Accept(visitor);
        }
    }
    visitor.EndVisit(*this);
}

BoundWhileStatement::BoundWhileStatement() : BoundParentStatement(nullptr)
{
}

BoundWhileStatement::BoundWhileStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundWhileStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    writer.Write(condition.get());
    writer.Write(statement.get());
}

void BoundWhileStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    Cm::Sym::BcuItem* condItem = reader.ReadItem();
    if (condItem->IsBoundExpression())
    {
        condition.reset(static_cast<BoundExpression*>(condItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* statementItem = reader.ReadItem();
    if (statementItem->IsBoundStatement())
    {
        statement.reset(static_cast<BoundStatement*>(statementItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
}

void BoundWhileStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundWhileStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statement.reset(statement_);
}

void BoundWhileStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    condition->Accept(visitor);
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundWhileStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) 
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundWhileStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundWhileStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundWhileStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

BoundDoStatement::BoundDoStatement() : BoundParentStatement(nullptr)
{
}

BoundDoStatement::BoundDoStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundDoStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    writer.Write(statement.get());
    writer.Write(condition.get());
}

void BoundDoStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    Cm::Sym::BcuItem* statementItem = reader.ReadItem();
    if (statementItem->IsBoundStatement())
    {
        statement.reset(static_cast<BoundStatement*>(statementItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
    Cm::Sym::BcuItem* condItem = reader.ReadItem();
    if (condItem->IsBoundExpression())
    {
        condition.reset(static_cast<BoundExpression*>(condItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
}

void BoundDoStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundDoStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    statement.reset(statement_);
}

void BoundDoStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        statement->Accept(visitor);
        condition->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundDoStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel) 
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundDoStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundDoStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundDoStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

BoundForStatement::BoundForStatement() : BoundParentStatement(nullptr)
{
}

BoundForStatement::BoundForStatement(Cm::Ast::Node* syntaxNode_) : BoundParentStatement(syntaxNode_)
{
}

void BoundForStatement::Write(Cm::Sym::BcuWriter& writer)
{
    BoundParentStatement::Write(writer);
    writer.Write(initS.get());
    writer.Write(condition.get());
    writer.Write(increment.get());
    writer.Write(action.get());
}

void BoundForStatement::Read(Cm::Sym::BcuReader& reader)
{
    BoundParentStatement::Read(reader);
    Cm::Sym::BcuItem* initItem = reader.ReadItem();
    if (initItem->IsBoundStatement())
    {
        initS.reset(static_cast<BoundStatement*>(initItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
    Cm::Sym::BcuItem* condItem = reader.ReadItem();
    if (condItem->IsBoundExpression())
    {
        condition.reset(static_cast<BoundExpression*>(condItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* incItem = reader.ReadItem();
    if (incItem->IsBoundExpression())
    {
        increment.reset(static_cast<BoundExpression*>(incItem));
    }
    else
    {
        throw std::runtime_error("bound expression expected");
    }
    Cm::Sym::BcuItem* actionItem = reader.ReadItem();
    if (actionItem->IsBoundStatement())
    {
        action.reset(static_cast<BoundStatement*>(actionItem));
    }
    else
    {
        throw std::runtime_error("bound statement expected");
    }
}

void BoundForStatement::SetCondition(BoundExpression* condition_)
{
    condition.reset(condition_);
}

void BoundForStatement::SetIncrement(BoundExpression* increment_)
{
    increment.reset(increment_);
}

void BoundForStatement::AddStatement(BoundStatement* statement_)
{
    statement_->SetParent(this);
    if (!initS)
    {
        initS.reset(statement_);
    }
    else
    {
        action.reset(statement_);
    }
}

void BoundForStatement::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (!visitor.SkipContent())
    {
        initS->Accept(visitor);
        condition->Accept(visitor);
        increment->Accept(visitor);
        action->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void BoundForStatement::AddBreakTargetLabel(Ir::Intf::LabelObject* breakTargetLabel)
{
    breakTargetLabels.push_back(breakTargetLabel);
}

void BoundForStatement::AddContinueTargetLabel(Ir::Intf::LabelObject* continueTargetLabel)
{
    continueTargetLabels.push_back(continueTargetLabel);
}

void BoundForStatement::AddToBreakNextSet(Cm::Core::CfgNode* node)
{
    breakNextSet.insert(node);
}

void BoundForStatement::AddToContinueNextSet(Cm::Core::CfgNode* node)
{
    continueNextSet.insert(node);
}

} } // namespace Cm::BoundTree
