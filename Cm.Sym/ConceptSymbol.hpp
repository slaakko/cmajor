/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CONCEPT_SYMBOL_INCLUDED
#define CM_SYM_CONCEPT_SYMBOL_INCLUDED
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Sym {

class ConceptSymbol : public ContainerSymbol
{
public:
    ConceptSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::conceptSymbol; }
    bool IsConceptSymbol() const override { return true; }
    const std::string& GroupName() const { return groupName; }
    void SetGroupName(const std::string& groupName_) { groupName = groupName_; }
    std::string TypeString() const override { return "concept"; };
    bool IsExportSymbol() const override;
    const std::vector<TypeParameterSymbol*>& TypeParameters() const { return typeParameters; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    void AddSymbol(Symbol* symbol) override;
    int Arity() const { return int(typeParameters.size()); }
    Cm::Ast::ConceptNode* GetConceptNode() const { return conceptNode.get(); }
    const TypeId& Id() const { return id; }
    ConceptSymbol* RefinedConcept() const { return refinedConcept; }
    void SetRefinedConcept(ConceptSymbol* refinedConcept_) { refinedConcept = refinedConcept_; }
    void SetIntrinsicConcept() { isIntrinsicConcept = true; }
    void Dump(CodeFormatter& formatter) override;
    std::string DocId() const override;
    std::string Syntax() const override;
private:
    bool isIntrinsicConcept;
    TypeId id;
    std::string groupName;
    std::vector<TypeParameterSymbol*> typeParameters;
    std::unique_ptr<Cm::Ast::ConceptNode> conceptNode;
    ConceptSymbol* refinedConcept;
};

TypeId ComputeInstantiatedConceptSymbolTypeId(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments);

class InstantiatedConceptSymbol : public Symbol
{
public:
    InstantiatedConceptSymbol(ConceptSymbol* conceptSymbol_, const std::vector<TypeSymbol*>& typeArguments_);
    SymbolType GetSymbolType() const override { return SymbolType::instantiatedConceptSymbol; }
    bool IsExportSymbol() const override { return false; }
    ConceptSymbol* Concept() const { return conceptSymbol; }
    const std::vector<TypeSymbol*>& TypeArguments() const { return typeArguments; }
    void SetCommonType(TypeSymbol* commonType_);
    TypeSymbol* CommonType() const { return commonType; }
    BcuItem* BoundConstraint() const { return boundConstraint.get(); }
    void SetBoundConstraint(BcuItem* boundConstraint_) { boundConstraint.reset(boundConstraint_); }
private:
    ConceptSymbol* conceptSymbol;
    std::vector<TypeSymbol*> typeArguments;
    TypeSymbol* commonType;
    std::unique_ptr<BcuItem> boundConstraint;
};

} } // namespace Cm::Sym

#endif // CM_SYM_CONCEPT_SYMBOL_INCLUDED
