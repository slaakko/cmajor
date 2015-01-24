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

namespace Cm { namespace Emit {

EmittingVisitor::EmittingVisitor(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_, 
    Cm::Core::IrClassTypeRepository& irClassTypeRepository_, Cm::Core::StringRepository& stringRepository_) :
    Cm::BoundTree::Visitor(false), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_), irClassTypeRepository(irClassTypeRepository_), stringRepository(stringRepository_), 
    irFile(irFilePath), codeFormatter(irFile), currentClass(nullptr)
{
    stringRepository.Write(codeFormatter);
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    irClassTypeRepository.Write(codeFormatter, compileUnit.SyntaxUnit(), externalFunctions, irFunctionRepository);
}

void EmittingVisitor::EndVisit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    for (Ir::Intf::Function* externalFunction : externalFunctions)
    {
        externalFunction->WriteDeclaration(codeFormatter, false, false);
    }
    staticMemberVariableRepository.Write(codeFormatter);
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundClass& boundClass)
{
    currentClass = &boundClass;
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = boundClass.Symbol();
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
    FunctionEmitter functionEmitter(codeFormatter, typeRepository, irFunctionRepository, irClassTypeRepository, stringRepository, currentClass, externalFunctions, staticMemberVariableRepository);
    boundFunction.Accept(functionEmitter);
}


} } // namespace Cm::Emit
