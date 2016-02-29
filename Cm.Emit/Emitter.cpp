/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/Emitter.hpp>
#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Core/GlobalSettings.hpp>
#include <Cm.Sym/GlobalFlags.hpp>

namespace Cm { namespace Emit {

Emitter::Emitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::IrInterfaceTypeRepository& irInterfaceTypeRepository_, Cm::Core::StringRepository& stringRepository_, 
    Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Cm::BoundTree::Visitor(false), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), 
    irInterfaceTypeRepository(irInterfaceTypeRepository_), stringRepository(stringRepository_),
    externalConstantRepository(externalConstantRepository_), irFile(irFilePath), codeFormatter(irFile), currentClass(nullptr), currentCompileUnit(nullptr), enterFrameFun(nullptr), leaveFrameFun(nullptr),
    enterTracedCallFun(nullptr), leaveTracedCallFun(nullptr), interfaceLookupFailed(nullptr), symbolTable(nullptr), profile(false), tpGraph(nullptr), nextTempTypedefNumber(0)
{
    Ir::Intf::SetCurrentTempTypedefProvider(this);
}

void Emitter::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    WriteCompileUnitHeader(codeFormatter);
    stringRepository.Write(codeFormatter);
    irClassTypeRepository.SetLayoutIndeces();
    if (Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::fullConfig))
    {
        compileUnit.ClassTemplateRepository().RetrieveMemberVariableLayoutIndecesFrom(irClassTypeRepository.ClassTypeMap());
        Cm::Core::StaticMemberVariableRepository& staticMemberVariableRepository = GetStaticMemberVariableRepository();
        staticMemberVariableRepository.SetClassTypeMap(irClassTypeRepository.ClassTypeMap());
    }
    currentCompileUnit = compileUnit.SyntaxUnit();
    enterFrameFun = compileUnit.SymbolTable().GetOverload("enter_frame");
    leaveFrameFun = compileUnit.SymbolTable().GetOverload("leave_frame");
    enterTracedCallFun = compileUnit.SymbolTable().GetOverload("enter_traced_call");
    leaveTracedCallFun = compileUnit.SymbolTable().GetOverload("leave_traced_call");
    interfaceLookupFailed = compileUnit.SymbolTable().GetOverload("System.Support.InterfaceLookupFailed");
    if (interfaceLookupFailed)
    {
        externalFunctions.insert(irFunctionRepository.CreateIrFunction(interfaceLookupFailed));
    }
    symbolTable = &compileUnit.SymbolTable();
    if (!compileUnit.IsMainUnit() && Cm::Core::GetGlobalSettings()->Config() == "profile")
    {
        profile = true;
    }
}

std::string Emitter::GetNextTempTypedefName()
{
    return "__tdf_X_" + std::to_string(nextTempTypedefNumber++);
}

} } // namespace Cm::Emit
