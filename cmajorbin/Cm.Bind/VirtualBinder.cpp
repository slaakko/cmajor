/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/VirtualBinder.hpp>
#include <Cm.Bind/SynthesizedClassFun.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Bind {

VirtualBinder::VirtualBinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Ast::CompileUnitNode* compileUnit_) : Cm::Ast::Visitor(false, false), symbolTable(symbolTable_), compileUnit(compileUnit_)
{
}

void VirtualBinder::EndVisit(Cm::Ast::ClassNode& classNode)
{
    Cm::Sym::ContainerScope* containerScope = symbolTable.GetContainerScope(&classNode);
    Cm::Sym::ClassTypeSymbol* classTypeSymbol = containerScope->Class();
    if (classTypeSymbol->DoGenerateDestructor())
    {
        Cm::Sym::FunctionSymbol* destructor = GenerateDestructorSymbol(symbolTable, classNode.GetSpan(), classTypeSymbol, compileUnit);
        classTypeSymbol->AddSymbol(destructor);
    }
    if (classTypeSymbol->DoGenerateStaticConstructor())
    {
        Cm::Sym::FunctionSymbol* staticConstructor = GenerateStaticConstructorSymbol(symbolTable, classNode.GetSpan(), classTypeSymbol, compileUnit);
        classTypeSymbol->AddSymbol(staticConstructor);
    }
    classTypeSymbol->InitVtbl();
}

} } // namespace Cm::Bind
