/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_TABLE_INCLUDED
#define CM_SYM_SYMBOL_TABLE_INCLUDED
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/TypeRepository.hpp>
#include <Cm.Sym/ConversionTable.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Delegate.hpp>
#include <Cm.Ast/Constant.hpp>
#include <Cm.Ast/Parameter.hpp>
#include <Cm.Ast/Statement.hpp>
#include <Cm.Ast/Typedef.hpp>
#include <Cm.Util/Uuid.hpp>
#include <stack>

namespace Cm { namespace Sym {

class ParameterSymbol;

class SymbolTable
{
public:
    SymbolTable();
    void BeginNamespaceScope(Cm::Ast::NamespaceNode* namespaceNode);
    void BeginNamespaceScope(const std::string& namespaceName, const Span& span);
    void EndNamespaceScope();
    void BeginClassScope(Cm::Ast::ClassNode* classNode);
    void EndClassScope();
    void BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode);
    void EndEnumScope();
    void AddEnumConstant(Cm::Ast::EnumConstantNode* enumConstantNode);
    void AddTypedef(Cm::Ast::TypedefNode* typedefNode);
    void BeginFunctionScope(Cm::Ast::FunctionNode* functionNode, FunctionSymbolFlags flags);
    void EndFunctionScope();
    void BeginDelegateScope(Cm::Ast::DelegateNode* delegateNode);
    void EndDelegateScope();
    void BeginClassDelegateScope(Cm::Ast::ClassDelegateNode* classDelegateNode);
    void EndClassDelegateScope();
    void AddConstant(Cm::Ast::ConstantNode* constantNode);
    void AddTemplateParameter(Cm::Ast::TemplateParameterNode* templateParameterNode);
    void AddParameter(Cm::Ast::ParameterNode* parameterNode, const std::string& parameterName);
    void AddParameter(ParameterSymbol* parameterSymbol);
    void BeginDeclarationScope(Cm::Ast::StatementNode* statementNode);
    void EndDeclarationcope();
    void AddLocalVariable(Cm::Ast::ConstructionStatementNode* constructionStatementNode);
    void AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode, int memberVariableIndex);
    ContainerScope* GlobalScope() { return globalNs.GetContainerScope(); }
    ContainerScope* GetContainerScope(Cm::Ast::Node* node) const;
    Cm::Ast::Node* GetNode(Symbol* symbol) const;
    Cm::Ast::Node* GetNode(Symbol* symbol, bool throw_) const;
    void SetNode(Symbol* symbol, Cm::Ast::Node* node);
    FunctionSymbol* GetFunctionSymbol(Cm::Ast::Node* functionNode) const;
    void Export(Writer& writer);
    void Import(Reader& reader);
    ContainerSymbol* Container() const { return container; }
    ClassTypeSymbol* CurrentClass() const { return currentClass; }
    FunctionSymbol* CurrentFunction() const { return currentFunction; }
    TypeRepository& GetTypeRepository() { return typeRepository; }
    ConversionTable& GetStandardConversionTable() { return standardConversionTable; }
    void AddPredefinedSymbolToGlobalScope(Symbol* symbol);
    void InitVirtualFunctionTables();
private:
    NamespaceSymbol globalNs;
    ContainerSymbol* container;
    ClassTypeSymbol* currentClass;
    FunctionSymbol* currentFunction;
    std::stack<ContainerSymbol*> containerStack;
    typedef std::unordered_map<Cm::Ast::Node*, ContainerScope*> NodeScopeMap;
    typedef NodeScopeMap::const_iterator NodeScopeMapIt;
    NodeScopeMap nodeScopeMap;
    typedef std::unordered_map<Symbol*, Cm::Ast::Node*> SymbolNodeMap;
    typedef SymbolNodeMap::const_iterator SymbolNodeMapIt;
    typedef std::unordered_map<Cm::Ast::Node*, FunctionSymbol*> NodeFunctionSymbolMap;
    typedef NodeFunctionSymbolMap::const_iterator NodeFunctionSymbolMapIt;
    NodeFunctionSymbolMap functionSymbolMap;
    TypeRepository typeRepository;
    ConversionTable standardConversionTable;
    SymbolNodeMap symbolNodeMap;
    void BeginContainer(ContainerSymbol* container_);
    void EndContainer();
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_TABLE_INCLUDED
