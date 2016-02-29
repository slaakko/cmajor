/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_PREBINDER_INCLUDED
#define CM_BIND_PREBINDER_INCLUDED
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.BoundTree/BoundFunction.hpp>
#include <Cm.BoundTree/BoundStatement.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

class Prebinder : public Cm::Ast::Visitor
{
public:
    Prebinder(Cm::Sym::SymbolTable& symbolTable_, Cm::Core::ClassTemplateRepository& classTemplateRepository_);
    void BeginCompileUnit();
    void EndCompileUnit();
    Cm::Sym::FileScope* ReleaseFileScope() { return fileScope.release(); }
    void BeginVisit(Cm::Ast::CompileUnitNode& compileUnitNode) override;
    void EndVisit(Cm::Ast::CompileUnitNode& compileUnitNode) override;
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override;
    void Visit(Cm::Ast::AliasNode& aliasNode) override;
    void Visit(Cm::Ast::NamespaceImportNode& namespaceImportNode) override;

    void BeginVisit(Cm::Ast::ClassNode& classNode) override;
    void EndVisit(Cm::Ast::ClassNode& classNode) override;
    void BeginVisit(Cm::Ast::InterfaceNode& interfaceNode) override;
    void EndVisit(Cm::Ast::InterfaceNode& interfaceNode) override;
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
    void Visit(Cm::Ast::MemberVariableNode& memberVariableNode) override;
    void BeginVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void EndVisit(Cm::Ast::EnumTypeNode& enumTypeNode) override;
    void Visit(Cm::Ast::EnumConstantNode& enumConstantNode) override;
    void Visit(Cm::Ast::TypedefNode& typedefNode) override;
    void Visit(Cm::Ast::TypedefStatementNode& typedefStatementNode) override;
    void Visit(Cm::Ast::ConstantNode& constantNode) override;
    void Visit(Cm::Ast::ParameterNode& parameterNode) override;

    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override;
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override;
    void BeginVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void EndVisit(Cm::Ast::DelegateNode& delegateNode) override;
    void BeginVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;
    void EndVisit(Cm::Ast::ClassDelegateNode& classDelegateNode) override;
    void BeginContainerScope(Cm::Sym::ContainerScope* containerScope);
    void EndContainerScope();
    void SetDontCompleteFunctions() { dontCompleteFunctions = true; }
    void SetCurrentClass(Cm::Sym::ClassTypeSymbol* currentClass_) { currentClass = currentClass_; }
private:
    Cm::Sym::SymbolTable& symbolTable;
    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
    Cm::Core::ClassTemplateRepository& classTemplateRepository;
    std::unique_ptr<Cm::Sym::FileScope> fileScope;
    Cm::Sym::ContainerScope* currentContainerScope;
    std::stack<Cm::Sym::ContainerScope*> containerScopeStack;
    int parameterIndex;
    Cm::Sym::ClassTypeSymbol* currentClass;
    std::stack<Cm::Sym::ClassTypeSymbol*> currentClassStack;
    Cm::Sym::FunctionSymbol* currentFunction;
    Cm::Sym::DelegateTypeSymbol* currentDelegate;
    Cm::Sym::ClassDelegateTypeSymbol* currentClassDelegate;
    std::vector<Cm::Ast::Node*> usingNodes;
    bool dontCompleteFunctions;
};

} } // namespace Cm::Bind

#endif // CM_BIND_PREBINDER_INCLUDED
