/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_INTERFACE_TYPE_SYMBOL_INCLUDED
#define CM_SYM_INTERFACE_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>

namespace Cm { namespace Sym {

const uint64_t noIid = -1;

class InterfaceCounter
{
public:
    InterfaceCounter();
    uint64_t GetIid() { return nextIid++; }
    void SetNextIid(uint64_t nextIid_) { nextIid = nextIid_; }
    uint64_t GetNextIid() const { return nextIid; }
private:
    uint64_t nextIid;
};

InterfaceCounter* GetInterfaceCounter();

void SetInterfaceCounter(InterfaceCounter* interfaceCounter_);

class InterfaceTypeSymbol : public TypeSymbol
{
public:
    InterfaceTypeSymbol(const Span& span_, const std::string& name_);
    bool IsInterfaceTypeSymbol() const override;
    void MakeIrType() override;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetIid(uint64_t iid_) { iid = iid_; }
    uint64_t Iid() const { return iid; }
    SymbolType GetSymbolType() const override { return SymbolType::interfaceTypeSymbol; }
    std::string TypeString() const override { return "interface"; };
    std::string GetMangleId() const override;
    Cm::Ast::CompileUnitNode* CompileUnit() const { return compileUnit; }
    void SetCompileUnit(Cm::Ast::CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    void AddSymbol(Symbol* symbol) override;
    const std::vector<FunctionSymbol*>& MemFuns() const { return memFuns; }
    const std::vector<MemberVariableSymbol*>& MemberVars() const { return memberVars; }
private:
    uint64_t iid;
    Cm::Ast::CompileUnitNode* compileUnit;
    std::vector<FunctionSymbol*> memFuns;
    std::vector<MemberVariableSymbol*> memberVars;
};

} } // namespace Cm::Sym

#endif // CM_SYM_INTERFACE_TYPE_SYMBOL_INCLUDED
