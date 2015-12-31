/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
#define CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>

namespace Cm { namespace Sym {

std::string MakeTemplateTypeSymbolName(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments);
TypeId ComputeTemplateTypeId(TypeSymbol* subjectType, const std::vector<TypeSymbol*>& typeArguments);

class TemplateTypeSymbol : public ClassTypeSymbol
{
public:
    TemplateTypeSymbol(const Span& span_, const std::string& name_);
    TemplateTypeSymbol(const Span& span_, const std::string& name_, TypeSymbol* subjectType_, const std::vector<TypeSymbol*>& typeArguments_, const TypeId& id_);
    SymbolType GetSymbolType() const override { return SymbolType::templateTypeSymbol; }
    bool IsTemplateTypeSymbol() const override { return true; }
    std::string TypeString() const override { return "template type"; };
    std::string GetMangleId() const override;
    bool IsExportSymbol() const override;
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void SetSubjectType(TypeSymbol* subjectType_);
    TypeSymbol* GetSubjectType() const { return subjectType; }
    const std::vector<TypeSymbol*>& TypeArguments() const { return typeArguments; }
    void AddTypeArgument(TypeSymbol* typeArgument);
    void SetType(TypeSymbol* type, int index) override;
    void MakeIrType() override;
    void RecomputeIrType() override;
    void SetFileScope(FileScope* fileScope_);
    FileScope* GetFileScope() const { return fileScope.get(); }
    FileScope* CloneFileScope() const { return fileScope->Clone(); }
    void SetGlobalNs(Cm::Ast::NamespaceNode* globalNs_);
    void CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes) override;
    void CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>& exportedTemplateTypes) override;
    void SetConstraint(Cm::Ast::WhereConstraintNode* constraint);
    Cm::Ast::WhereConstraintNode* GetConstraint() const { return constraint.get(); }
    std::string FullDocId() const override;
    void SetPrimaryTemplateTypeSymbol(TemplateTypeSymbol* primaryTemplateTypeSymbol_) { primaryTemplateTypeSymbol = primaryTemplateTypeSymbol_; }
    TemplateTypeSymbol* GetPrimaryTemplateTypeSymbol() const { return primaryTemplateTypeSymbol; }
    void ReplaceReplicaTypes() override;
    void DoSerialize() override;
private:
    TypeSymbol* subjectType;
    std::vector<TypeSymbol*> typeArguments;
    std::unique_ptr<Cm::Ast::NamespaceNode> globalNs;
    std::unique_ptr<FileScope> fileScope;
    std::unique_ptr<Cm::Ast::WhereConstraintNode> constraint;
    TemplateTypeSymbol* primaryTemplateTypeSymbol;
};

} } // namespace Cm::Sym

#endif // CM_SYM_TEMPLATE_TYPE_SYMBOL_INCLUDED
