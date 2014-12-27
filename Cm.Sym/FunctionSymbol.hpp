/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_SYMBOL_INCLUDED
#define CM_SYM_FUNCTION_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Ast/Function.hpp>

namespace Cm { namespace Sym {

class FunctionLookup
{
public:
    FunctionLookup(ScopeLookup lookup_, ContainerScope* scope_);
    ScopeLookup Lookup() const { return lookup; }
    void SetLookup(ScopeLookup lookup_) { lookup = lookup_; }
    ContainerScope* Scope() const { return scope; }
private:
    ScopeLookup lookup;
    ContainerScope* scope;
};

inline bool operator==(const FunctionLookup& left, const FunctionLookup& right)
{
    return left.Lookup() == right.Lookup() && left.Scope() == right.Scope();
}

class FunctionLookupSet
{
public:
    FunctionLookupSet();
    typedef std::vector<FunctionLookup>::const_iterator const_iterator;
    void Add(const FunctionLookup& lookup);
    const_iterator begin() const { return lookups.begin(); }
    const_iterator end() const { return lookups.end(); }
private:
    std::vector<FunctionLookup> lookups;
};

class FunctionSymbol : public ContainerSymbol
{
public:
    FunctionSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::functionSymbol; }
    const std::string& GroupName() const { return groupName; }
    void SetGroupName(const std::string& groupName_) { groupName = groupName_; }
    std::string TypeString() const override { return "function"; };
    bool IsFunctionSymbol() const override { return true; }
    virtual bool IsBasicTypeOp() const { return false; }
    virtual bool IsConvertingConstructor() const { return false; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void AddSymbol(Symbol* symbol) override;
    void SetReturnType(TypeSymbol* returnType_);
    TypeSymbol* GetReturnType() const { return returnType; }
    int Arity() const { return int(parameters.size()); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    void ComputeName();
    virtual TypeSymbol* GetTargetType() const;
private:
    std::string groupName;
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_SYMBOL_INCLUDED
