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
    irFile(irFilePath), codeFormatter(irFile)
{
    stringRepository.Write(codeFormatter);
}

void EmittingVisitor::Visit(Cm::BoundTree::BoundCompileUnit& compileUnit)
{
    irClassTypeRepository.Write(codeFormatter);
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsExternal()) return;
    FunctionEmitter functionEmitter(codeFormatter, typeRepository, irFunctionRepository, irClassTypeRepository, stringRepository);
    boundFunction.Accept(functionEmitter);
}

void EmittingVisitor::Visit(Cm::BoundTree::BoundClass& boundClass)
{
    irClassTypeRepository.AddClassType(boundClass.Symbol());
}

} } // namespace Cm::Emit
