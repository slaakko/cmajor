/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/FunctionGroupSymbol.hpp>

namespace Cm { namespace Sym {

FunctionGroupSymbol::FunctionGroupSymbol(const Span& span_, const std::string& name_, ContainerScope* containerScope_) : Symbol(span_, name_), containerScope(containerScope_)
{
}

void FunctionGroupSymbol::AddFunction(FunctionSymbol* function)
{
    if (function->GroupName() != Name())
    {
        throw std::runtime_error("attempt to insert a function with group name '" + function->GroupName() + "' to wrong function group '" + Name() + "'");
    }
    int arity = function->Arity();
    std::vector<FunctionSymbol*>& functionList = arityFunctionListMap[arity];
    functionList.push_back(function);
}

void FunctionGroupSymbol::CollectViableFunctions(int arity, std::unordered_set<FunctionSymbol*>& viableFunctions)
{
    ArityFunctionListMapIt i = arityFunctionListMap.find(arity);
    if (i != arityFunctionListMap.end())
    {
        std::vector<FunctionSymbol*>& functionList = i->second;
        for (FunctionSymbol* function : functionList)
        {
            viableFunctions.insert(function);
        }
    }
}

} } // namespace Cm::Sym
