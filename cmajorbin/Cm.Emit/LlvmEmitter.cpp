/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/LlvmEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Emit/LlvmFunctionEmitter.hpp>

namespace Cm { namespace Emit {

LlvmEmitter::LlvmEmitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Emitter(irFilePath, typeRepository_, irFunctionRepository_, irClassTypeRepository_, stringRepository_, externalConstantRepository_)
{
}

void LlvmEmitter::WriteCompileUnitHeader(Cm::Util::CodeFormatter& codeFormatter)
{
    // nothing to do for LLVM backend
}

void LlvmEmitter::EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    for (Ir::Intf::Function* function : ExternalFunctions())
    {
        if (InternalFunctionNames().find(function->Name()) == InternalFunctionNames().end())
        {
            function->WriteDeclaration(CodeFormatter(), false, false);
        }
    }
    staticMemberVariableRepository.Write(CodeFormatter());
    ExternalConstantRepository().Write(CodeFormatter());
}

void LlvmEmitter::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    SetCurrentClass(&boundClass);
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = boundClass.Symbol();
    if (ProcessedClasses().find(classTypeSymbol) != ProcessedClasses().end()) return;
    ProcessedClasses().insert(classTypeSymbol);
    for (Cm::Sym::MemberVariableSymbol* staticMemberVarSymbol : classTypeSymbol->StaticMemberVariables())
    {
        staticMemberVariableRepository.Add(staticMemberVarSymbol);
    }
    if (classTypeSymbol->InitializedVar())
    {
        staticMemberVariableRepository.Add(classTypeSymbol->InitializedVar());
    }
}

void LlvmEmitter::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsExternal()) return;
    LlvmFunctionEmitter functionEmitter(CodeFormatter(), TypeRepository(), IrFunctionRepository(), IrClassTypeRepository(), StringRepository(), CurrentClass(), InternalFunctionNames(), 
        ExternalFunctions(), staticMemberVariableRepository, ExternalConstantRepository(), CurrentCompileUnit(), EnterFrameFun(), LeaveFrameFun(), EnterTracedCallFun(), LeaveTracedCallFun());
    functionEmitter.SetSymbolTable(SymbolTable());
    boundFunction.Accept(functionEmitter);
}

} } // namespace Cm::Emit
