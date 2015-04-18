/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_C_FUNCTION_EMITTER_INCLUDED
#define CM_EMIT_C_FUNCTION_EMITTER_INCLUDED
#include <Cm.Emit/FunctionEmitter.hpp>

namespace Cm { namespace Emit {

class CFunctionEmitter : public FunctionEmitter
{
public:
    CFunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_,
        std::unordered_set<std::string>& internalFunctionNames_, std::unordered_set<Ir::Intf::Function*>& externalFunctions_,
        Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_,
        Cm::Ast::CompileUnitNode* currentCompileUnit_, Cm::Sym::FunctionSymbol* enterFrameFun_, Cm::Sym::FunctionSymbol* leaveFrameFun_, Cm::Sym::FunctionSymbol* enterTracedCalllFun_,
        Cm::Sym::FunctionSymbol* leaveTracedCallFun_);
    void SetFunctionMap(std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*>* functionMap_) { functionMap = functionMap_; }
    void EmitDummyVar(Cm::Core::Emitter* emitter) override;
    void SetStringLiteralResult(Cm::Core::Emitter* emitter, Ir::Intf::Object* resultObject, Ir::Intf::Object* stringConstant, Ir::Intf::Object* stringObject) override;
    void Visit(Cm::BoundTree::BoundDynamicTypeNameExpression& boundDynamiceTypeNameExpression) override;
    void DoNothing(Cm::Core::GenResult& genResult) override;
    void Visit(Cm::BoundTree::BoundInitVPtrStatement& boundInitVPtrStatement) override;
    void RegisterDestructor(Cm::Sym::MemberVariableSymbol* staticMemberVariableSymbol) override;
    void GenVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& memberFunctionResult) override;
    Ir::Intf::LabelObject* CreateLandingPadLabel(int landingPadId) override;
    void MapIrFunToFun(Ir::Intf::Function* irFun, Cm::Sym::FunctionSymbol* fun) override;
    std::unordered_map<Ir::Intf::Function*, Cm::Sym::FunctionSymbol*>* functionMap;
};

} } // namespace Cm::Emit

#endif // CM_EMIT_C_FUNCTION_EMITTER_INCLUDED

