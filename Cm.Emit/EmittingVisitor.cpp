/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundClass.hpp>
#include <Llvm.Ir/Type.hpp>

namespace Cm { namespace Emit {

EmittingVisitor::EmittingVisitor(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_, Cm::Core::ExternalConstantRepository& externalConstantRepository_) :
    Cm::BoundTree::Visitor(false), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_),
    externalConstantRepository(externalConstantRepository_), irFile(irFilePath), codeFormatter(irFile), currentClass(nullptr), enterFrameFun(nullptr), leaveFrameFun(nullptr)
{
    stringRepository.Write(codeFormatter);
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
	irClassTypeRepository.Write(codeFormatter, compileUnit.SyntaxUnit(), externalFunctions, irFunctionRepository);
    currentCompileUnit = compileUnit.SyntaxUnit();
	enterFrameFun = compileUnit.SymbolTable().GetOverload("enter_frame");
	leaveFrameFun = compileUnit.SymbolTable().GetOverload("leave_frame");
}

void EmittingVisitor::EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    for (Ir::Intf::Function* function : externalFunctions)
    {
        if (internalFunctionNames.find(function->Name()) == internalFunctionNames.end())
        {
            function->WriteDeclaration(codeFormatter, false, false);
        }
    }
    staticMemberVariableRepository.Write(codeFormatter);
    externalConstantRepository.Write(codeFormatter);
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    currentClass = &boundClass;
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = boundClass.Symbol();
    if (processedClasses.find(classTypeSymbol) != processedClasses.end()) return;
    processedClasses.insert(classTypeSymbol);
    for (Cm::Sym::MemberVariableSymbol* staticMemberVarSymbol : classTypeSymbol->StaticMemberVariables())
    {
        staticMemberVariableRepository.Add(staticMemberVarSymbol);
    }
    if (classTypeSymbol->InitializedVar())
    {
        staticMemberVariableRepository.Add(classTypeSymbol->InitializedVar());
    }
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsExternal()) return;
	FunctionEmitter functionEmitter(codeFormatter, typeRepository, irFunctionRepository, irClassTypeRepository, stringRepository, currentClass, internalFunctionNames, externalFunctions, 
		staticMemberVariableRepository, externalConstantRepository, currentCompileUnit, enterFrameFun, leaveFrameFun);
	boundFunction.Accept(functionEmitter);
}

} } // namespace Cm::Emit
