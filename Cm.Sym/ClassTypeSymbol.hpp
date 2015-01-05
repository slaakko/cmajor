/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CLASS_SYMBOL_INCLUDED
#define CM_SYM_CLASS_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Sym {

class MemberVariableSymbol;

class ClassTypeSymbol : public TypeSymbol
{
public:
    ClassTypeSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::classSymbol; }
    std::string TypeString() const override { return "class"; };
    std::string GetMangleId() const override;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    bool IsClassTypeSymbol() const override { return true; }
    ClassTypeSymbol* BaseClass() const { return baseClass; }
    void SetBaseClass(ClassTypeSymbol* baseClass_) { baseClass = baseClass_; }
    bool HasBaseClass(ClassTypeSymbol* cls) const;
    bool HasBaseClass(ClassTypeSymbol* cls, int& distance) const;
    void AddSymbol(Symbol* symbol) override;
    const std::vector<MemberVariableSymbol*>& MemberVariables() const { return memberVariables; }
private:
    ClassTypeSymbol* baseClass;
    std::vector<MemberVariableSymbol*> memberVariables;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CLASS_SYMBOL_INCLUDED
