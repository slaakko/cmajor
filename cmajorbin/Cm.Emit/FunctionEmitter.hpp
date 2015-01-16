/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_EMIT_FUNCTION_EMITTER_INCLUDED
#define CM_EMIT_FUNCTION_EMITTER_INCLUDED
#include <Cm.BoundTree/Visitor.hpp>
#include <Cm.Core/GenData.hpp>
#include <Cm.Core/IrFunctionRepository.hpp>
#include <Cm.Core/IrClassTypeRepository.hpp>
#include <Cm.Core/StringRepository.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>

namespace Cm { namespace Emit {

class LocalVariableIrObjectRepository
{
public:
    Ir::Intf::Object* CreateLocalVariableIrObjectFor(Cm::Sym::Symbol *localVariableOrParameter);
    Ir::Intf::Object* GetLocalVariableIrObject(Cm::Sym::Symbol *localVariableOrParameter);
private:
    typedef std::unordered_map<Cm::Sym::Symbol*, Ir::Intf::Object*>  LocalVariableObjectMap;
    typedef LocalVariableObjectMap::const_iterator LocalVariableObjectMapIt;
    LocalVariableObjectMap localVariableObjectMap;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedIrObjects;
    std::unordered_set<std::string> assemblyNames;
    std::string MakeUniqueAssemblyName(const std::string& name);
};

class IrObjectRepository
{
public:
    Ir::Intf::Object* MakeMemberVariableIrObject(Cm::BoundTree::BoundMemberVariable* boundMemberVariable, Ir::Intf::Object* ptr);
private:
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedIrObjects;
};

class FunctionEmitter : public Cm::BoundTree::Visitor
{
public:
    FunctionEmitter(Cm::Util::CodeFormatter& codeFormatter_, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
        Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::BoundTree::BoundClass* currentClass_, 
        std::unordered_set<Ir::Intf::Function*>& externalFunctions_);
    void BeginVisit(Cm::BoundTree::BoundFunction& boundFunction) override;
    void EndVisit(Cm::BoundTree::BoundFunction& boundFunction) override;

    void Visit(Cm::BoundTree::BoundLiteral& boundLiteral) override;
    void Visit(Cm::BoundTree::BoundStringLiteral& boundStringLiteral) override;
    void Visit(Cm::BoundTree::BoundConstant& boundConstant) override;
    void Visit(Cm::BoundTree::BoundLocalVariable& boundLocalVariable) override;
    void Visit(Cm::BoundTree::BoundParameter& boundParameter) override;
    void Visit(Cm::BoundTree::BoundMemberVariable& boundMemberVariable) override;
    void Visit(Cm::BoundTree::BoundConversion& boundConversion) override;
    void Visit(Cm::BoundTree::BoundCast& boundCast) override;
    void Visit(Cm::BoundTree::BoundUnaryOp& boundUnaryOp) override;
    void Visit(Cm::BoundTree::BoundBinaryOp& boundBinaryOp) override;
    void Visit(Cm::BoundTree::BoundFunctionCall& functionCall) override;
    void Visit(Cm::BoundTree::BoundDisjunction& boundDisjunction) {}
    void Visit(Cm::BoundTree::BoundConjunction& boundConjunction) {}

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
    void Visit(Cm::BoundTree::BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(Cm::BoundTree::BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(Cm::BoundTree::BoundThrowStatement& boundThrowStatement) {}
    void Visit(Cm::BoundTree::BoundSimpleStatement& boundSimpleStatement) override;
    void Visit(Cm::BoundTree::BoundSwitchStatement& boundSwitchStatement) {}
    void Visit(Cm::BoundTree::BoundBreakStatement& boundBreakStatement) {}
    void Visit(Cm::BoundTree::BoundContinueStatement& boundContinueStatement) {}
    void BeginVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement) override;
    void EndVisit(Cm::BoundTree::BoundConditionalStatement& boundConditionalStatement) override;
    void BeginVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement) override;
    void EndVisit(Cm::BoundTree::BoundWhileStatement& boundWhileStatement) override;
    void BeginVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement) override;
    void EndVisit(Cm::BoundTree::BoundDoStatement& boundDoStatement) override;
    void BeginVisit(Cm::BoundTree::BoundForStatement& boundForStatement) override;
    void EndVisit(Cm::BoundTree::BoundForStatement& boundForStatement) override;
    void Visit(Cm::BoundTree::BoundTryStatement& boundTryStatement) {}
private:
    Cm::Util::CodeFormatter& codeFormatter;
    std::unique_ptr<Cm::Core::Emitter> emitter;
    Cm::Sym::TypeRepository& typeRepository;
    Cm::Core::GenFlags genFlags;
    Cm::Core::GenResult compoundResult;
    Cm::Core::GenResultStack resultStack;
    Cm::Core::IrFunctionRepository& irFunctionRepository;
    Cm::Core::IrClassTypeRepository& irClassTypeRepository;
    Cm::Core::StringRepository& stringRepository;
    LocalVariableIrObjectRepository localVariableIrObjectRepository;
    IrObjectRepository irObjectRepository;
    Cm::Ast::CompileUnitNode* currentCompileUnit;
    Cm::BoundTree::BoundClass* currentClass;
    Cm::Sym::ParameterSymbol* thisParam;
    std::unordered_set<Ir::Intf::Function*>& externalFunctions;
    void GenerateCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& result);
    void GenerateVirtualCall(Cm::Sym::FunctionSymbol* fun, Cm::Core::GenResult& result);
    void GenerateCall(Ir::Intf::Function* fun, Cm::Core::GenResult& result, bool constructorOrDestructorCall);
    void GenJumpingBoolCode(Cm::Core::GenResult& result);
};

} } // namespace Cm::Emit

#endif // CM_EMIT_FUNCTION_EMITTER_INCLUDED