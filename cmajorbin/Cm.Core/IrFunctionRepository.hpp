/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
#define CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Ir.Intf/Function.hpp>

namespace Cm { namespace Core {

class IrFunctionRepository
{
public:
    IrFunctionRepository();
    Ir::Intf::Function* GetDoNothingFunction();
    Ir::Intf::Function* GetMemSetFunction();
    Ir::Intf::Function* CreateIrFunction(Cm::Sym::FunctionSymbol* function);
    Ir::Intf::Object* GetFunctionId(Cm::Sym::FunctionSymbol* function, Cm::Sym::TypeSymbol* functionPtrPtrType);
    Ir::Intf::Type* CreateIrPointerToDelegateType(Cm::Sym::DelegateTypeSymbol* delegateType);
    Ir::Intf::Type* GetFunPtrIrType(Cm::Sym::FunctionSymbol* fun);
    Ir::Intf::Parameter* GetExceptionCodeParam() const { return exceptionCodeParam; }
    Ir::Intf::Parameter* CreateIrParameter(Cm::Sym::ParameterSymbol* parameter);
private:
    typedef std::unordered_map<Cm::Sym::FunctionSymbol*, Ir::Intf::Function*> IrFunctionMap;
    typedef IrFunctionMap::const_iterator IrFunctionMapIt;
    typedef std::unordered_map<Cm::Sym::FunctionSymbol*, Ir::Intf::Type*> IrFunTypeMap;
    typedef IrFunTypeMap::const_iterator IrFunTypeMapIt;
    typedef std::unordered_map<Cm::Sym::FunctionSymbol*, Ir::Intf::Object*> FunctionIdMap;
    typedef FunctionIdMap::const_iterator FunctionIdMapIt;
    IrFunctionMap irFunctionMap;
    IrFunTypeMap irFunPtrMap;
    FunctionIdMap functionIdMap;
    std::unique_ptr<Ir::Intf::Function> doNothingFunction;
    std::unique_ptr<Ir::Intf::Function> memSetFunction;
    std::vector<std::unique_ptr<Ir::Intf::Function>> ownedIrFunctions;
    std::vector<std::unique_ptr<Ir::Intf::Type>> ownedIrTypes;
    std::vector<std::unique_ptr<Ir::Intf::Object>> ownedObjects;
    std::vector<std::unique_ptr<Ir::Intf::Parameter>> ownedIrParameters;
    Ir::Intf::Parameter* exceptionCodeParam;
    void Own(Ir::Intf::Type* type);
    void Own(Ir::Intf::Object* object);
    void Own(Ir::Intf::Parameter* parameter);
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_FUNCTION_REPOSITORY_INCLUDED
