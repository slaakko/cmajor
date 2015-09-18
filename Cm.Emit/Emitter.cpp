/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/Emitter.hpp>
#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Cm.Core/GlobalSettings.hpp>

namespace Cm { namespace Emit {

Emitter::Emitter(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_,
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Cm::BoundTree::Visitor(false), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_),
    externalConstantRepository(externalConstantRepository_), irFile(irFilePath), codeFormatter(irFile), currentClass(nullptr), enterFrameFun(nullptr), leaveFrameFun(nullptr),
    enterTracedCallFun(nullptr), leaveTracedCallFun(nullptr), symbolTable(nullptr), profile(false)
{
}

void Emitter::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    WriteCompileUnitHeader(codeFormatter);
    stringRepository.Write(codeFormatter);
    irClassTypeRepository.Write(codeFormatter, externalFunctions, irFunctionRepository);
    currentCompileUnit = compileUnit.SyntaxUnit();
    enterFrameFun = compileUnit.SymbolTable().GetOverload("enter_frame");
    leaveFrameFun = compileUnit.SymbolTable().GetOverload("leave_frame");
    enterTracedCallFun = compileUnit.SymbolTable().GetOverload("enter_traced_call");
    leaveTracedCallFun = compileUnit.SymbolTable().GetOverload("leave_traced_call");
    symbolTable = &compileUnit.SymbolTable();
    if (!compileUnit.IsMainUnit() && Cm::Core::GetGlobalSettings()->Config() == "profile")
    {
        profile = true;
    }
}

} } // namespace Cm::Emit
