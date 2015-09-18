/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
#define CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
#include <Cm.Sym/VariableSymbol.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Sym {

class TypeSymbol;

class MemberVariableSymbol : public VariableSymbol
{
public:
    MemberVariableSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::memberVariableSymbol; }
    std::string TypeString() const override { return "member variable"; };
    bool IsMemberVariableSymbol() const override { return true; }
    bool IsExportSymbol() const override;
    int LayoutIndex() const { return layoutIndex; }
    void SetLayoutIndex(int layoutIndex_) { layoutIndex = layoutIndex_; }
	void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
	void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, TemplateTypeSymbol*, TypeIdHash>& exportedTemplateTypes) override;
    void Dump(CodeFormatter& formatter) override;
    void ReplaceReplicaTypes() override;
private:
    int layoutIndex;
};

} } // namespace Cm::Sym

#endif // CM_SYM_MEMBER_VARIABLE_SYMBOL_INCLUDED
