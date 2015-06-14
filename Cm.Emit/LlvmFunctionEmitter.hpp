/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_LLVM_FUNCTION_EMITTER_INCLUDED
#define CM_EMIT_LLVM_FUNCTION_EMITTER_INCLUDED
#include <Cm.Emit/FunctionEmitter.hpp>

namespace Cm { namespace Emit {

class LlvmFunctionEmitter : public FunctionEmitter
{
public:
    LlvmFunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_,
        std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_,
        Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_,
        Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_,
        Cm::Sym::FunctionSymbol* leaveTracedCallFun_);
    void EmitDummyVar(Cm::Core::Emitter* emitter) override;
    void SetStringLiteralResult(Cm::Core::Emitter* emitter, Ir::Intf::Object* resultObject, Ir::Intf::Object* stringConstant, Ir::Intf::Object* stringObject) override;
    void Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamiceTypeNameExpression) override;
    void DoNothing(Cm::Core::GenResult& genResult) override;
    void Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement) override;
    void RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol) override;
    void GenVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& memberFunctionResult) override;
    Ir::Intf::LabelObject* CreateLandingPadLabel(int landingPadId) override;
    void MapIrFunToFun(Ir::Intf::Function* irFun, Cm::Sym::FunctionSymbol* fun) override;
    Ir::Intf::Object* MakeLocalVarIrObject(Cm::Sym::TypeSymbol* type, Ir::Intf::Object* source) override;
    void SetCallDebugInfoInfo(Ir::Intf::Instruction* callInst, Ir::Intf::Function* fun) override;

    Cm::Core::CfgNode* CreateDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span, bool addToPrevNodes) override;
    void CreateDebugNode(Cm::BoundTree::BoundExpression& expr, const Cm::Parsing::Span& span) override;
    void AddDebugNodeTransition(Cm::BoundTree::BoundStatement& fromStatement, Cm::BoundTree::BoundStatement& toStatement) override;
    void AddDebugNodeTransition(Cm::BoundTree::BoundExpression& fromExpression, Cm::BoundTree::BoundStatement& toStatement) override;
    void AddDebugNodeTransition(Cm::BoundTree::BoundExpression& fromExpression, Cm::BoundTree::BoundExpression& toExpression) override;
    int RetrievePrevDebugNodes() override;
    void AddToPrevDebugNodes(int debugNodeSetHandle) override;
    void AddToPrevDebugNodes(Cm::BoundTree::BoundStatement& statement) override;
    void AddToPrevDebugNodes(Cm::BoundTree::BoundExpression& expr) override;
    void AddToPrevDebugNodes(const std::unordered_set<Cm::Core::CfgNode*>& nodeSet) override;
    void CreateEntryDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span) override;
    void CreateExitDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span) override;
    void PatchPrevDebugNodes(Cm::BoundTree::BoundStatement& statement) override;
    void SetCfgNode(Cm::BoundTree::BoundStatement& fromStatement, Cm::BoundTree::BoundStatement& toStatement) override;
    void PatchDebugNodes(const std::unordered_set<Cm::Core::CfgNode*>& nodeSet, Cm::Core::CfgNode* nextNode) override;
};

} } // namespace Cm::Emit

#endif // CM_EMIT_LLVM_FUNCTION_EMITTER_INCLUDED
