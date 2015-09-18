/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_PARAMETER_SYMBOL_INCLUDED
#define CM_SYM_PARAMETER_SYMBOL_INCLUDED
#include <Cm.Sym/VariableSymbol.hpp>
#include <Cm.Ast/Parameter.hpp>

namespace Cm { namespace Sym {

class TypeSymbol;

class ParameterSymbol : public VariableSymbol
{
public:
    ParameterSymbol(const Span& span_, const std::string& name_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    SymbolType GetSymbolType() const override { return SymbolType::parameterSymbol; }
    std::string TypeString() const override { return "parameter"; };
    bool IsParameterSymbol() const override { return true; }
    bool IsExportSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, TemplateTypeSymbol*, TypeIdHash>& exportedTemplateTypes) override;
    Cm::Ast::ParameterNode* ParameterNode() const { return ownedParameterNode.get(); }
    void ReplaceReplicaTypes() override;
private:
    std::unique_ptr<Cm::Ast::ParameterNode> ownedParameterNode;
};

} } // namespace Cm::Sym

#endif // CM_SYM_PARAMETER_SYMBOL_INCLUDED
