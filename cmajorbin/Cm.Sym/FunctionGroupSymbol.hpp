/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
#define CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>
#include <unordered_set>

namespace Cm { namespace Sym {

class FunctionGroupSymbol : public Symbol
{
public:
    FunctionGroupSymbol(const Span& span_, const std::string& name_, ContainerScope* containerScope_);
    SymbolType GetSymbolType() const override { return SymbolType::functionGroupSymbol; }
    bool IsFunctionGroupSymbol() const override { return true; }
    std::string TypeString() const override { return "function group"; };
    void AddFunction(FunctionSymbol* function);
    void CollectViableFunctions(int arity, std::unordered_set<FunctionSymbol*>& viableFunctions);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    ContainerScope* GetContainerScope() override { return containerScope; }
private:
    std::string name;
    typedef std::unordered_map<int, std::vector<FunctionSymbol*>> ArityFunctionListMap;
    typedef ArityFunctionListMap::iterator ArityFunctionListMapIt;
    ArityFunctionListMap arityFunctionListMap;
    ContainerScope* containerScope;
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
