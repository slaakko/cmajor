/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DECLARATION_VISITOR_INCLUDED
#define CM_SYM_DECLARATION_VISITOR_INCLUDED
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Sym {

class DeclarationVisitor : public Cm::Ast::Visitor
{
public:
    DeclarationVisitor(SymbolTable& symbolTable_);
    void SetCidMap(std::unordered_map<std::string, uint64_t>* cidMap_);
    void SetTemplateType(Cm::Ast::ClassNode* templateClassNode_, Cm::Sym::TemplateTypeSymbol* templateType_);
    void MarkFunctionSymbolAsTemplateSpecialization() { markFunctionSymbolAsTemplateSpecialization = true; }
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void BeginVisit(Cm::Ast::ClassNode& classNode) override;
    void EndVisit(Cm::Ast::ClassNode& classNode) override;
    void BeginVisit(Cm::Ast::ConstructorNode& constructorNode) override;
    void EndVisit(Cm::Ast::ConstructorNode& constructorNode) override;
    void BeginVisit(Cm::Ast::DestructorNode& destructorNode) override;
    void EndVisit(Cm::Ast::DestructorNode& destructorNode) override;
    void BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override;
    void EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override;
    void BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override;
    void EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override;
    void BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override;
    void EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override;
    void BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void Visit(Cm::Ast::EnumConstantNode& enumConstantNode) override;
    void Visit(Cm::Ast::TypedefNode& typedefNode) override;
    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override;
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override;
    void BeginVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void EndVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;
    void EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;
    void BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override;
    void BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;
    void EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override;
    void BeginVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override;
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override;
    void Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode) override;
    void Visit(Cm::Ast::ConstantNode& constantNode) override;
    void Visit(Cm::Ast::TemplateParameterNode& templateParameterNode) override;
    void Visit(Cm::Ast::ParameterNode& parameterNode) override;
    void Visit(Cm::Ast::MemberVariableNode& memberVariableNode) override;
    void Visit(Cm::Ast::ConceptNode& conceptNode) override;
private:
    SymbolTable& symbolTable;
    int parameterIndex;
    Cm::Ast::ClassNode* templateClassNode;
    Cm::Sym::TemplateTypeSymbol* templateType;
    bool markFunctionSymbolAsTemplateSpecialization;
    std::unordered_map<std::string, uint64_t>* cidMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_DECLARATION_VISITOR_INCLUDED
