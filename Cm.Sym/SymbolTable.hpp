/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Sym/LocalVariableSymbol.hpp>
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
class ConceptSymbol;

class SymbolTable
{
public:
    SymbolTable();
    SymbolTable(const SymbolTable& that);
    void BeginNamespaceScope(Cm::Ast::NamespaceNode* namespaceNode);
    void BeginNamespaceScope(const std::string& namespaceName, const Span& span);
    void EndNamespaceScope();
    void BeginClassScope(Cm::Ast::ClassNode* classNode, std::unordered_map<std::string, uint64_t>* cidMap);
    void EndClassScope();
    void BeginTemplateTypeScope(Cm::Ast::ClassNode* templateClassNode, Cm::Sym::TemplateTypeSymbol* templateTypeSymbol);
    void EndTemplateTypeScope();
    void BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode);
    void EndEnumScope();
    void AddEnumConstant(Cm::Ast::EnumConstantNode* enumConstantNode);
    void AddTypedef(Cm::Ast::TypedefNode* typedefNode);
    void AddTypedef(Cm::Ast::TypedefStatementNode* typedefStatementNode);
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
    void AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode);
    void AddEntrySymbol();
    void AddReturnValueSymbol(Cm::Ast::Node* returnTypeExpr);
    ConceptSymbol* BeginConceptScope(Cm::Ast::ConceptNode* conceptNode);
    void EndConceptScope();
    ContainerScope* GlobalScope() { return globalNs.GetContainerScope(); }
    ContainerScope* GetContainerScope(Cm::Ast::Node* node) const;
    Cm::Ast::Node* GetNode(Symbol* symbol) const;
    Cm::Ast::Node* GetNode(Symbol* symbol, bool throw_) const;
    void SetNode(Symbol* symbol, Cm::Ast::Node* node);
    FunctionSymbol* GetFunctionSymbol(Cm::Ast::Node* functionNode) const;
    void Export(Writer& writer);
    void Import(Reader& reader);
    void Import(Reader& reader, bool importTypeRepository);
    ContainerSymbol* Container() const { return container; }
    ClassTypeSymbol* CurrentClass() const { return currentClass; }
    void SetCurrentClass(ClassTypeSymbol* currentClass_) { currentClass = currentClass_; }
    FunctionSymbol* CurrentFunction() const { return currentFunction; }
    TypeRepository& GetTypeRepository() { return typeRepository; }
    ConversionTable& GetStandardConversionTable() { return standardConversionTable; }
    void AddPredefinedSymbolToGlobalScope(Symbol* symbol);
    void InitVirtualFunctionTables();
    FunctionSymbol* GetOverload(const std::string& fullOverloadGroupName) const;
    void BeginContainer(ContainerSymbol* container_);
    void EndContainer();
    Cm::Sym::FunctionSymbol* UserMainFunction() const { return userMainFunction; }
    void SetUserMainFunction(Cm::Sym::FunctionSymbol* userMainFunction_) { userMainFunction = userMainFunction_; }
    NamespaceSymbol& GlobalNs() { return globalNs; }
    void AddImportedTemplateType(TemplateTypeSymbol* importedTemplateType) { importedTemplateTypes.push_back(importedTemplateType); }
    void ProcessImportedTemplateTypes();
    uint32_t GetNextSid() const { return nextSid; }
    void SetNextSid(uint32_t nextSid_) { nextSid = nextSid_; }
    void AddSymbol(Symbol* symbol);
    uint32_t GetSid();
    void SetSidAndAddSymbol(Symbol* symbol);
    Symbol* GetSymbol(uint32_t sid);
    ClassTypeSymbol* GetClass(uint64_t cid) const;
    void Own(LocalVariableSymbol* localVariable);
    const std::unordered_set<ClassTypeSymbol*>& Classes() const;
    const std::unordered_set<ClassTypeSymbol*>& ProjectClasses() const;
    uint32_t GetVariableSymbolSid(const std::string& variableSymbolFullName);
    std::unordered_map<uint64_t, uint64_t>& CidMap() { return cidMap; }
    void SetVirtualClassCid(const std::string& className, uint64_t cid);
    uint64_t GetVirtualClassCid(const std::string& className) const;
private:
    uint32_t nextSid;
    NamespaceSymbol globalNs;
    ContainerSymbol* container;
    std::stack<ContainerSymbol*> containerStack;
    ClassTypeSymbol* currentClass;
    std::stack<ClassTypeSymbol*> currentClassStack;
    FunctionSymbol* currentFunction;
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
    Cm::Sym::FunctionSymbol* userMainFunction;
    std::vector<TemplateTypeSymbol*> importedTemplateTypes;
    std::unordered_map<uint32_t, Symbol*> symbolMap;
    std::unordered_map<uint64_t, ClassTypeSymbol*> classMap;
    std::unordered_map<uint64_t, uint64_t> cidMap;
    std::unordered_map<std::string, uint64_t> classNameCidMap;
    std::unordered_set<ClassTypeSymbol*> classes;
    std::unordered_set<ClassTypeSymbol*> projectClasses;
    std::vector<std::unique_ptr<LocalVariableSymbol>> ownedLocalVariables;
    std::vector<uint64_t> classHierarchyTable;
    std::unordered_map<std::string, uint32_t> variableSymbolNameSidMap;
};

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_TABLE_INCLUDED
