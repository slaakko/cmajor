/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>

namespace Cm { namespace Bind {

VirtualBinder::VirtualBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Ast::CompileUnitNode* compileUnit_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : 
    Cm::Ast::Visitor(false, false), symbolTable(symbolTable_), compileUnit(compileUnit_), boundCompileUnit(boundCompileUnit_)
{
}

void VirtualBinder::BindTemplateTypeSymbols(Cm::Sym::ClassTypeSymbol* classTypeSymbol, Cm::Sym::ContainerScope* containerScope)
{
    if (classTypeSymbol->BaseClass())
    {
        BindTemplateTypeSymbols(classTypeSymbol->BaseClass(), containerScope);
    }
    if (classTypeSymbol->IsTemplateTypeSymbol() && !classTypeSymbol->Bound())
    {
        Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(classTypeSymbol);
        boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, containerScope, boundCompileUnit.GetFileScopes());
    }
}

void VirtualBinder::EndVisit(Cm::Ast::ClassNode& classNode)
{
    if (classNode.TemplateParameters().Count() > 0) return;
    Cm::Sym::ContainerScope* containerScope = symbolTable.GetContainerScope(&classNode);
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = containerScope->Class();
    BindTemplateTypeSymbols(classTypeSymbol, containerScope);
    if (classTypeSymbol->DoGenerateDestructor())
    {
        Cm::Sym::FunctionSymbol* destructor = GenerateDestructorSymbol(symbolTable, classNode.GetSpan(), classTypeSymbol, compileUnit);
        destructor->SetPublic();
        classTypeSymbol->AddSymbol(destructor);
    }
    if (classTypeSymbol->DoGenerateStaticConstructor())
    {
        Cm::Sym::FunctionSymbol* staticConstructor = GenerateStaticConstructorSymbol(symbolTable, classNode.GetSpan(), classTypeSymbol, compileUnit);
        staticConstructor->SetPublic();
        classTypeSymbol->AddSymbol(staticConstructor);
    }
    classTypeSymbol->InitVtbl();
}

} } // namespace Cm::Bind
