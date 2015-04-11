/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_FUNCTION_EMITTER_INCLUDED
#define CM_EMIT_FUNCTION_EMITTER_INCLUDED
#include <Cm.Bind/ClassDelegateTypeOpRepository.hpp>
#include <Cm.BoundTree/BoundExpression.hpp>
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Core/GenData.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Core/StaticMemberVariableRepository.hpp>
#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Core/StringRepository.hpp>
#include <Cm.Core/ExternalConstantRepository.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>

namespace Cm { namespace Emit {

class LocalVariableIrObjectRepository
{
public:
    LocalVariableIrObjectRepository(Cm::Core::IrFunctionRepository* irFunctionRepository_);
    Ir::Intf::Object* CreateLocalVariableIrObjectFor(Cm::Sym::Symbol *localVariableOrParameter);
    Ir::Intf::Object* GetLocalVariableIrObject(Cm::Sym::Symbol *localVariableOrParameter);
    void SetExceptionCodeVariable(Ir::Intf::Object* exceptionCodeVariable_) { exceptionCodeVariable = exceptionCodeVariable_; }
    Ir::Intf::Object* GetExceptionCodeVariable() const { return exceptionCodeVariable; }
private:
    Cm::Core::IrFunctionRepository* irFunctionRepository;
    typedef std::unordered_map<Cm::Sym::Symbol*, Ir::Intf::Object*>  LocalVariableObjectMap;
    typedef LocalVariableObjectMap::const_iterator LocalVariableObjectMapIt;
    LocalVariableObjectMap localVariableObjectMap;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedIrObjects;
    std::unordered_set<std::string> assemblyNames;
    std::string MakeUniqueAssemblyName(const std::string& name);
    Ir::Intf::Object* exceptionCodeVariable;
};

class IrObjectRepository
{
public:
    IrObjectRepository();
    Ir::Intf::Object* MakeMemberVariableIrObject(Cm::BoundTree::BoundMemberVariable* boundMemberVariable, Ir::Intf::Object* ptr);
    void Write(Cm::Util::CodeFormatter& codeFormatter);
private:
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedIrObjects;
};

enum class SwitchEmitState
{
    none, createSwitchTargets, emitStatements
};

class CompoundDestructionStack
{
public:
    void Push(Cm::BoundTree::BoundDestructionStatement* destructionStatement);
    std::unique_ptr<Cm::BoundTree::BoundDestructionStatement> Pop();
    const std::vector<std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>>& DestructionStatements() const { return destructionStatements; }
    bool IsEmpty() const { return destructionStatements.empty(); }
private:
    std::vector<std::unique_ptr<Cm::BoundTree::BoundDestructionStatement>> destructionStatements;
};

class FunctionDestructionStack
{
public:
    void Push(CompoundDestructionStack&& compoundDestructionStack);
    CompoundDestructionStack Pop();
    const std::vector<CompoundDestructionStack>& CompoundDestructionStacks() const { return compoundDestructionStacks; }
    bool IsEmpty() const { return compoundDestructionStacks.empty(); }
private:
    std::vector<CompoundDestructionStack> compoundDestructionStacks;
};

class FunctionEmitter : public Cm::BoundTree::Visitor
{
public:
    FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_, 
        std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_, 
        Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_, 
        Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_, 
        Cm::Sym::FunctionSymbol* leaveTracedCallFun_);
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    void EndVisit(Cm::BoundTree::BoundFunction& boundFunction) override;

    void Visit(Cm::BoundTree::BoundLiteral& boundLiteral) override;
    void Visit(Cm::BoundTree::BoundStringLiteral& boundStringLiteral) override;
    void Visit(Cm::BoundTree::BoundConstant& boundConstant) override;
    void Visit(Cm::BoundTree::BoundEnumConstant& boundEnumConstant) override;
    void Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable) override;
    void Visit(Cm::BoundTree::BoundParameter& boundParameter) override;
    void Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable) override;
    void Visit(Cm::BoundTree::BoundFunctionId& boundFunctionId) override;
    void Visit(Cm::BoundTree::BoundConversion& boundConversion) override;
    void Visit(Cm::BoundTree::BoundCast& boundCast) override;
    void Visit(Cm::BoundTree::BoundSizeOfExpression& boundSizeOfExpr) override;
    void Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamiceTypeNameExpression) override;
    void Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp) override;
    void Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& functionCall) override;
    void Visit(Cm::BoundTree::BoundDelegateCall& boundDelegateCall) override;
    void Visit(Cm::BoundTree::BoundClassDelegateCall& boundClassDelegateCall) override;
    void Visit(Cm::BoundTree::BoundDisjunction& boundDisjunction) override;
    void Visit(Cm::BoundTree::BoundConjunction& boundConjunction) override;
    void Visit(Cm::BoundTree::BoundPostfixIncDecExpr& boundPostfixIncDecExpr) override;
    void BeginVisitStatement(Cm::BoundTree::BoundStatement& statement) override;
    void EndVisitStatement(Cm::BoundTree::BoundStatement& statement) override;
    void BeginVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement) override;
    void EndVisit(Cm::BoundTree::BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(Cm::BoundTree::BoundReceiveStatement& boundReceiveStatement) override;
    void Visit(Cm::BoundTree::BoundInitClassObjectStatement& boundInitClassObjectStatement) override;
    void Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement) override;
    void Visit(Cm::BoundTree::BoundInitMemberVariableStatement& boundInitMemberVariableStatement) override;
    void Visit(Cm::BoundTree::BoundFunctionCallStatement& boundFunctionCallStatement) override;
    void Visit(Cm::BoundTree::BoundReturnStatement& boundReturnStatement) override;
    void Visit(Cm::BoundTree::BoundBeginTryStatement& boundBeginTryStatement) override;
    void Visit(Cm::BoundTree::BoundEndTryStatement& boundEndTryStatement) override;
    void Visit(Cm::BoundTree::BoundExitBlocksStatement& boundExitBlocksStatement) override;
    void Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(Cm::BoundTree::BoundDestructionStatement& boundDestructionStatement) override;
    void Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement) override;
    void Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement) override;
    void Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) override;
    void Visit(Cm::BoundTree::BoundGotoStatement& boundGotoStatement) override;
    void BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement) override;
    void EndVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement) override;
    void BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement) override;
    void EndVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement) override;
    void BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement) override;
    void EndVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement) override;
    void BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement) override;
    void EndVisit(Cm::BoundTree::BoundForStatement& boundForStatement) override;
    void Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(Cm::BoundTree::BoundCaseStatement& boundCaseStatement) override;
    void Visit(Cm::BoundTree::BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(Cm::BoundTree::BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(Cm::BoundTree::BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
private:
	std::unique_ptr<Cm::Core::Emitter> emitter;
    Cm::Util::CodeFormatter& codeFormatter;
    Cm::Sym::TypeRepository& typeRepository;
    Cm::Core::GenFlags genFlags;
    std::shared_ptr<Cm::Core::GenResult> compoundResult;
    Cm::Core::GenResultStack compoundResultStack;
    Cm::Core::GenResultStack resultStack;
    bool firstStatementInCompound;
    FunctionDestructionStack functionDestructionStack;
    CompoundDestructionStack currentCompoundDestructionStack;
    std::stack<bool> firstStatementInCompoundStack;
    Cm::Core::IrFunctionRepository& irFunctionRepository;
    Cm::Core::IrClassTypeRepository& irClassTypeRepository;
    Cm::Core::StringRepository& stringRepository;
    LocalVariableIrObjectRepository localVariableIrObjectRepository;
    IrObjectRepository irObjectRepository;
    Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository;
    Cm::Core::ExternalConstantRepository& externalConstantRepository;
    Cm::Ast::CompileUnitNode* currentCompileUnit;
    Cm::BoundTree::BoundClass* currentClass;
    Cm::BoundTree::BoundFunction* currentFunction;
    Cm::Sym::ParameterSymbol* thisParam;
    std::unordered_set<std::string>& internalFunctionNames;
    std::unordered_set<Ir::Intf::Function*>& externalFunctions;
    std::vector<std::unique_ptr<Cm::BoundTree::BoundStatement>> postfixIncDecStatements;
    Cm::BoundTree::BoundStatement* continueTargetStatement;
    std::stack<Cm::BoundTree::BoundStatement*> continueTargetStatementStack;
    Cm::BoundTree::BoundStatement* breakTargetStatement;
    std::stack<Cm::BoundTree::BoundStatement*> breakTargetStatementStack;
    bool executingPostfixIncDecStatements;
    SwitchEmitState currentSwitchEmitState;
    std::stack<SwitchEmitState> switchEmitStateStack;
    int currentCatchId;
    std::stack<int> catchIdStack;
    typedef std::unordered_map<std::string, std::pair<Ir::Intf::LabelObject*, Cm::BoundTree::BoundStatement*>> SwitchCaseConstantMap;
    typedef SwitchCaseConstantMap::const_iterator SwitchCaseConstantMapIt;
    std::stack<SwitchCaseConstantMap*> switchCaseConstantMapStack;
    SwitchCaseConstantMap* currentSwitchCaseConstantMap;
    Ir::Intf::LabelObject* switchCaseLabel;
    std::vector<Ir::Intf::Object*> switchCaseConstants;
	Cm::Sym::FunctionSymbol* enterFrameFun;
	Cm::Sym::FunctionSymbol* leaveFrameFun;
    Cm::Sym::FunctionSymbol* enterTracedCallFun;
    Cm::Sym::FunctionSymbol* leaveTracedCallFun;
	void ClearCompoundDestructionStack(Cm::Core::GenResult& result);
    void ExitCompound(Cm::Core::GenResult& result, const CompoundDestructionStack& compoundDestructionStack, bool& first);
    void ExitCompounds(Cm::BoundTree::BoundCompoundStatement* fromCompound, Cm::BoundTree::BoundCompoundStatement* targetCompound, Cm::Core::GenResult& result);
    void ExitFunction(Cm::Core::GenResult& result);
    void PushBreakTargetStatement(Cm::BoundTree::BoundStatement* statement);
    void PopBreakTargetStatement();
    void PushContinueTargetStatement(Cm::BoundTree::BoundStatement* statement);
    void PopContinueTargetStatement();
    void MakePlainValueResult(Cm::Sym::TypeSymbol* plainType, Cm::Core::GenResult& result);
    void ExecutePostfixIncDecStatements(Cm::Core::GenResult& result);
    void GenerateCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result);
    void GenerateVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result);
    void GenerateCall(Cm::Sym::FunctionSymbol* functionSymbol, Ir::Intf::Function* fun, Cm::BoundTree::TraceCallInfo* traceCallInfo, Cm::Core::GenResult& result, bool constructorOrDestructorCall);
    void GenJumpingBoolCode(Cm::Core::GenResult& result);
    void CallEnterFrame(Cm::BoundTree::TraceCallInfo* traceCallInfo);
    void CallLeaveFrame(Cm::BoundTree::TraceCallInfo* traceCallInfo);
    void RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol);
    void GenerateTestExceptionResult();
    void CreateLandingPad(int landingPadId);
    void GenerateLandingPadCode();
    void GenerateClassDelegateInitFromFun(Cm::Bind::ClassDelegateFromFunCtor* ctor, Cm::Core::GenResult& result);
    void GenerateClassDelegateAssignmentFromFun(Cm::Bind::ClassDelegateFromFunAssignment* assignment, Cm::Core::GenResult& result);
};

} } // namespace Cm::Emit

#endif // CM_EMIT_FUNCTION_EMITTER_INCLUDED
