/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_C_FUNCTION_EMITTER_INCLUDED
#define CM_EMIT_C_FUNCTION_EMITTER_INCLUDED
#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.Core/CDebugInfo.hpp>
#include <C.Ir/Function.hpp>

namespace Cm { namespace Emit {

class CFunctionEmitter : public FunctionEmitter
{
public:
    CFunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_,
        std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_,
        Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_,
        Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_,
        Cm::Sym::FunctionSymbol* leaveTracedCallFun_, const char* start_, const char* end_, bool generateDebugInfo_, bool profile_);
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    Ir::Intf::Type* CFunctionEmitter::ReplaceFunctionPtrType(Ir::Intf::Type* localVariableIrType) override;
    void EndVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    void SetFunctionMap(std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*>* functionMap_) { functionMap = functionMap_; }
    void EmitDummyVar(Cm::Core::Emitter* emitter) override;
    void SetStringLiteralResult(Cm::Core::Emitter* emitter, Ir::Intf::Object* resultObject, Ir::Intf::Object* stringConstant, Ir::Intf::Object* stringObject) override;
    void Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamicTypeNameExpression) override;
    void Visit(Cm::BoundTree::BoundIsExpression& boundIsExpression) override;
    void Visit(Cm::BoundTree::BoundAsExpression& boundAsExpression) override;
    void DoNothing(Cm::Core::GenResult& genResult) override;
    void Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement) override;
    void RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol) override;
    void GenVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& memberFunctionResult) override;
    void SetCallDebugInfoInfo(Ir::Intf::Instruction* callInst, Ir::Intf::Function* fun) override;
    Ir::Intf::LabelObject* CreateLandingPadLabel(int landingPadId) override;
    void MapIrFunToFun(Ir::Intf::Function* irFun, Cm::Sym::FunctionSymbol* fun) override;
    Ir::Intf::Object* MakeLocalVarIrObject(Cm::Sym::TypeSymbol* type, Ir::Intf::Object* source) override;
    void Visit(Cm::BoundTree::BoundBeginThrowStatement& boundBeginThrowStatement) override;
    void Visit(Cm::BoundTree::BoundEndThrowStatement& boundEndThrowStatement) override;
    void Visit(Cm::BoundTree::BoundBeginCatchStatement& boundBeginCatchStatement) override;
    Cm::Core::CFunctionDebugInfo* ReleaseFunctionDebugInfo() { return functionDebugInfo.release(); }

    Cm::Core::CfgNode* CreateDebugNode(Cm::BoundTree::BoundStatement& statement, const Cm::Parsing::Span& span, bool addToPrevSet) override;
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
    void SetCFilePath(const std::string& cFilePath_) { cFilePath = cFilePath_; }
    Cm::Core::CFunctionDebugInfo* GetFunctionDebugInfo() const override { return functionDebugInfo.get(); }
    std::vector<std::unique_ptr<C::Typedef>> Tdfs() { return std::move(tdfs); }
private:
    std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*>* functionMap;
    bool generateDebugInfo;
    std::unique_ptr<Cm::Core::CFunctionDebugInfo> functionDebugInfo;
    const char* start;
    const char* end;
    std::vector<std::unordered_set<Cm::Core::CfgNode*>> debugNodeSets;
    std::string cFilePath;
    std::vector<std::unique_ptr<C::Typedef>> tdfs;
};

} } // namespace Cm::Emit

#endif // CM_EMIT_C_FUNCTION_EMITTER_INCLUDED

