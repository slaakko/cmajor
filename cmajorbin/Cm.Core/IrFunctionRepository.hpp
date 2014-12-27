/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
#define CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Ir.Intf/Function.hpp>

namespace Cm { namespace Core {

class IrFunctionRepository
{
public:
    Ir::Intf::Function* CreateIrFunction(Cm::Sym::FunctionSymbol* function);
private:
    typedef std::unordered_map<Cm::Sym::FunctionSymbol*, Ir::Intf::Function*> IrFunctionMap;
    typedef IrFunctionMap::const_iterator IrFunctionMapIt;
    IrFunctionMap irFunctionMap;
    std::vector<std::unique_ptr<Ir::Intf::Function>> ownedIrFunctions;
    std::vector<std::unique_ptr<Ir::Intf::Type>> ownedIrTypes;
    std::vector<std::unique_ptr<Ir::Intf::Parameter>> ownedIrParameters;
    void Own(Ir::Intf::Type* type);
    void Own(Ir::Intf::Parameter* parameter);
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
