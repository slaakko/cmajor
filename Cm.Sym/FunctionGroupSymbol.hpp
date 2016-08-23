/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
#define CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <unordered_set>

namespace Cm { namespace Sym {

class FunctionGroupSymbol : public Symbol
{
public:
    FunctionGroupSymbol(const Span& span_, const std::string& name_, ContainerScope* containerScope_);
    bool IsExportSymbol() const override { return false; }
    SymbolType GetSymbolType() const override { return SymbolType::functionGroupSymbol; }
    bool IsFunctionGroupSymbol() const override { return true; }
    std::string TypeString() const override { return "function group"; };
    bool IsCCSymbol() const override;
    void AddFunction(FunctionSymbol* function);
    void CollectViableFunctions(int arity, std::unordered_set<FunctionSymbol*>& viableFunctions);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    ContainerScope* GetContainerScope() const override { return const_cast<ContainerScope*>(containerScope); }
    FunctionSymbol* GetOverload() const;
    void Dump(CodeFormatter& formatter) override;
    const std::vector<TypeSymbol*>& BoundTemplateArguments() const { return boundTemplateArguments; }
    void SetBoundTemplateArguments(const std::vector<TypeSymbol*>& boundTemplateArguments_);
    void CollectSymbolsForCC(std::unordered_set<Symbol*>& ccSymbols);
private:
    typedef std::unordered_map<int, std::vector<FunctionSymbol*>> ArityFunctionListMap;
    typedef ArityFunctionListMap::iterator ArityFunctionListMapIt;
    ArityFunctionListMap arityFunctionListMap;
    ContainerScope* containerScope;
    std::vector<TypeSymbol*> boundTemplateArguments;
};

class FunctionGroupTypeSymbol : public TypeSymbol
{
public:
    FunctionGroupTypeSymbol(Cm::Sym::FunctionGroupSymbol* functionGroupSymbol_);
    SymbolType GetSymbolType() const override { return SymbolType::functionGroupTypeSymbol; }
    std::string GetMangleId() const override { return Name(); }
    Cm::Sym::FunctionGroupSymbol* GetFunctionGroupSymbol() const { return functionGroupSymbol; }
    bool IsFunctionGroupTypeSymbol() const override { return true; }
private:
    Cm::Sym::FunctionGroupSymbol* functionGroupSymbol;
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_GROUP_SYMBOL_INCLUDED
