/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Emit/EmittingVisitor.hpp>
#include <Cm.Emit/FunctionEmitter.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>

namespace Cm { namespace Emit {

EmittingVisitor::EmittingVisitor(const std::string& irFilePath, Cm::Sym::TypeRepository& typeRepository_, Cm::Core::IrFunctionRepository& irFunctionRepository_) : 
    Cm::BoundTree::Visitor(false), typeRepository(typeRepository_), irFunctionRepository(irFunctionRepository_), irFile(irFilePath), codeFormatter(irFile)
{
}

void EmittingVisitor::BeginVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
    FunctionEmitter functionEmitter(codeFormatter, typeRepository, irFunctionRepository);
    boundFunction.Accept(functionEmitter);
}

void EmittingVisitor::EndVisit(Cm::BoundTree::BoundFunction& boundFunction)
{
}

} } // namespace Cm::Emit
