/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/CodeCompletion.hpp>
#include <Cm.Bind/LocalVariable.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Ast/Expression.hpp>
#include <algorithm>
#include <fstream>
#include <map>
#include <stack>

namespace Cm { namespace Bind {

class CodeCompletionVisitor : public Cm::Ast::Visitor
{
public:
    CodeCompletionVisitor(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    void BeginVisit(Cm::Ast::NamespaceNode& namespaceNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&namespaceNode));
    }
    void EndVisit(Cm::Ast::NamespaceNode& namespaceNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::ClassNode& classNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&classNode));
        currentClassSymbol = containerScope->Class();
        Cm::Ast::ConstraintNode* constraint = classNode.Constraint();
        if (constraint)
        {
            includeConceptSymbolsStack.push(includeConceptSymbols);
            includeConceptSymbols = true;
            constraint->Accept(*this);
            includeConceptSymbols = includeConceptSymbolsStack.top();
            includeConceptSymbolsStack.pop();
        }
    }
    void EndVisit(Cm::Ast::ClassNode& classNode) override
    {
        currentClassSymbol = nullptr;
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::InterfaceNode& interfaceNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&interfaceNode));
    }
    void EndVisit(Cm::Ast::InterfaceNode& interfaceNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::ConstructorNode& constructorNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&constructorNode));
    }
    void EndVisit(Cm::Ast::ConstructorNode& constructorNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::DestructorNode& destructorNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&destructorNode));
    }
    void EndVisit(Cm::Ast::DestructorNode& destructorNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&memberFunctionNode));
    }
    void EndVisit(Cm::Ast::MemberFunctionNode& memberFunctionNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&conversionFunctionNode));
    }
    void EndVisit(Cm::Ast::ConversionFunctionNode& conversionFunctionNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&staticConstructorNode));
    }
    void EndVisit(Cm::Ast::StaticConstructorNode& staticConstructorNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::FunctionNode& functionNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&functionNode));
        Cm::Ast::ConstraintNode* constraint = functionNode.Constraint();
        if (constraint)
        {
            includeConceptSymbolsStack.push(includeConceptSymbols);
            includeConceptSymbols = true;
            constraint->Accept(*this);
            includeConceptSymbols = includeConceptSymbolsStack.top();
            includeConceptSymbolsStack.pop();
        }
    }
    void Visit(Cm::Ast::DisjunctiveConstraintNode& disjunctiveConstraintNode) override
    {
        disjunctiveConstraintNode.Left()->Accept(*this);
        disjunctiveConstraintNode.Right()->Accept(*this);
    }
    void Visit(Cm::Ast::ConjunctiveConstraintNode& conjunctiveConstraintNode) override
    {
        conjunctiveConstraintNode.Left()->Accept(*this);
        conjunctiveConstraintNode.Right()->Accept(*this);
    }
    void Visit(Cm::Ast::PredicateConstraintNode& predicateConstraintNode) override
    {
        predicateConstraintNode.InvokeExpr()->Accept(*this);
    }
    void Visit(Cm::Ast::IsConstraintNode& isConstraintNode) override
    {
        isConstraintNode.TypeExpr()->Accept(*this);
        isConstraintNode.ConceptOrTypeName()->Accept(*this);
    }
    void Visit(Cm::Ast::MultiParamConstraintNode& multiParamConstraintNode) override
    {
        multiParamConstraintNode.ConceptId()->Accept(*this);
        multiParamConstraintNode.TypeExprNodes().Accept(*this);
    }
    void Visit(Cm::Ast::ConceptIdNode& conceptIdNode) override
    {
        conceptIdNode.Id()->Accept(*this);
    }
    void EndVisit(Cm::Ast::FunctionNode& functionNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&compoundStatementNode));
    }
    void EndVisit(Cm::Ast::CompoundStatementNode& compoundStatementNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::ForStatementNode& forStatementNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&forStatementNode));
    }
    void EndVisit(Cm::Ast::ForStatementNode& forStatementNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override
    {
        BeginContainerScope(symbolTable.GetContainerScope(&rangeForStatementNode));
    }
    void EndVisit(Cm::Ast::RangeForStatementNode& rangeForStatementNode) override
    {
        EndContainerScope();
    }
    void BeginVisit(Cm::Ast::ConstructionStatementNode& constructionStatementNode) override
    {
        std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
        fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope->Clone()));
        BindLocalVariable(symbolTable, containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), boundCompileUnit, &constructionStatementNode);
    }
    void Visit(Cm::Ast::TryStatementNode& tryStatementNode) override
    {
        tryStatementNode.TryBlock()->Accept(*this);
        tryStatementNode.Handlers().Accept(*this);
    }
    void Visit(Cm::Ast::CatchNode& catchNode) override
    {
        catchNode.CatchBlock()->Accept(*this);
    }
    void Visit(Cm::Ast::CCNode& ccNode) override
    {
        PushLookup(Cm::Sym::ScopeLookup::this_and_base_and_parent);
        Cm::Sym::SymbolTypeSetId typeSet = Cm::Sym::SymbolTypeSetId::lookupAllSymbols;
        if (ccNode.Parent()->IsClassNode() || ccNode.Parent()->IsNamespaceNode())
        {
            typeSet = Cm::Sym::SymbolTypeSetId::lookupTypeAndNamespaceSymbols;
        }
        PushTypeSet(typeSet);
        PushCollectFileScope(true);
        Collect();
        PopCollectFileScope();
        PopTypeSet();
        PopLookup();
    }
    void BeginVisit(Cm::Ast::DotNode& dotNode) override
    {
        PushTypeSet(Cm::Sym::SymbolTypeSetId::lookupDotSubjectSymbols);
    }
    void EndVisit(Cm::Ast::DotNode& dotNode) override
    {
        PopTypeSet();
        Cm::Sym::Symbol* symbol = symbolStack.top();
        symbolStack.pop();
        if (symbol)
        {
            Cm::Sym::ContainerScope* scope = nullptr;
            if (symbol->IsVariableSymbol())
            {
                Cm::Sym::VariableSymbol* variableSymbol = static_cast<Cm::Sym::VariableSymbol*>(symbol);
                if (variableSymbol->IsLocalVariableSymbol())
                {
                    Cm::Sym::LocalVariableSymbol* localVariableSymbol = static_cast<Cm::Sym::LocalVariableSymbol*>(variableSymbol);
                    Cm::Ast::Node* node = symbolTable.GetNode(localVariableSymbol);
                    if (!node->IsConstructionStatementNode())
                    {
                        throw std::runtime_error("construction statement node expected");
                    }
                    Cm::Ast::ConstructionStatementNode* constructionStatementNode = static_cast<Cm::Ast::ConstructionStatementNode*>(node);
                    std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
                    fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope->Clone()));
                    BindLocalVariable(symbolTable, containerScope, fileScopes, boundCompileUnit.ClassTemplateRepository(), boundCompileUnit, constructionStatementNode, localVariableSymbol);
                }
                Cm::Sym::TypeSymbol* type = variableSymbol->GetType();
                if (type)
                {
                    type = symbolTable.GetTypeRepository().MakePlainType(type);
                    if (type->IsTemplateTypeSymbol())
                    {
                        Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(type);
                        if (!templateTypeSymbol->Bound())
                        {
                            std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
                            fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope->Clone()));
                            boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, containerScope, fileScopes);
                        }
                    }
                    scope = type->GetContainerScope();
                }
            }
            else if (symbol->IsNamespaceSymbol() || symbol->IsTypeSymbol())
            {
                scope = symbol->GetContainerScope();
            }
            if (scope)
            {
                if (dotNode.MemberStr() == "`")
                {
                    BeginContainerScope(scope);
                    PushCollectFileScope(false);
                    PushLookup(Cm::Sym::ScopeLookup::this_and_base);
                    Cm::Sym::SymbolTypeSetId typeSet = Cm::Sym::SymbolTypeSetId::lookupAllSymbols;
                    if (dotNode.Parent()->IsClassNode() || dotNode.Parent()->IsNamespaceNode())
                    {
                        typeSet = Cm::Sym::SymbolTypeSetId::lookupTypeAndNamespaceSymbols;
                    }
                    PushTypeSet(typeSet);
                    Collect();
                    PopTypeSet();
                    PopLookup();
                    PopCollectFileScope();
                    EndContainerScope();
                }
                else
                {
                    symbol = scope->Lookup(dotNode.MemberStr(), Cm::Sym::ScopeLookup::this_and_base, typeSet);
                    symbolStack.push(symbol);
                }
            }
        }
    }
    void Visit(Cm::Ast::ArrowNode& arrowNode) override
    {
        PushTypeSet(Cm::Sym::SymbolTypeSetId::lookupAllSymbols);
        arrowNode.Subject()->Accept(*this);
        PopTypeSet();
        Cm::Sym::Symbol* symbol = symbolStack.top();
        symbolStack.pop();
        if (symbol)
        {
            Cm::Sym::TypeSymbol* type = symbol->GetType();
            while (type)
            {
                if (type->IsClassTypeSymbol())
                {
                    if (type->IsTemplateTypeSymbol())
                    {
                        Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(type);
                        if (!templateTypeSymbol->Bound())
                        {
                            std::vector<std::unique_ptr<Cm::Sym::FileScope>> fileScopes;
                            fileScopes.push_back(std::unique_ptr<Cm::Sym::FileScope>(fileScope->Clone()));
                            boundCompileUnit.ClassTemplateRepository().BindTemplateTypeSymbol(templateTypeSymbol, containerScope, fileScopes);
                        }
                    }
                    Cm::Sym::Symbol* opArrowSymbol = type->GetContainerScope()->Lookup("operator->", Cm::Sym::ScopeLookup::this_and_base);
                    if (opArrowSymbol && opArrowSymbol->IsFunctionGroupSymbol())
                    {
                        Cm::Sym::FunctionGroupSymbol* opArrowGroup = static_cast<Cm::Sym::FunctionGroupSymbol*>(opArrowSymbol);
                        Cm::Sym::FunctionSymbol* opArrow = opArrowGroup->GetOverload();
                        if (opArrow)
                        {
                            type = opArrow->GetReturnType();
                        }
                        else
                        {
                            type = nullptr;
                        }
                    }
                    else
                    {
                        throw std::runtime_error("function group symbol expected");
                    }
                }
                else
                {
                    if (type->IsPointerToClassType())
                    {
                        type = type->GetBaseType();
                        if (arrowNode.MemberStr() == "`")
                        {
                            BeginContainerScope(type->GetContainerScope());
                            PushCollectFileScope(false);
                            PushLookup(Cm::Sym::ScopeLookup::this_and_base);
                            PushTypeSet(Cm::Sym::SymbolTypeSetId::lookupAllSymbols);
                            Collect();
                            PopTypeSet();
                            PopLookup();
                            PopCollectFileScope();
                            EndContainerScope();
                        }
                        else
                        {
                            symbol = type->GetContainerScope()->Lookup(arrowNode.MemberStr(), Cm::Sym::ScopeLookup::this_and_base, typeSet);
                            symbolStack.push(symbol);
                        }
                    }
                    break;
                }
            }
        }
    }
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override
    {
        Cm::Sym::Symbol* symbol = containerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, typeSet);
        if (!symbol)
        {
            symbol = fileScope->Lookup(identifierNode.Str(), typeSet);
        }
        symbolStack.push(symbol);
    }
    std::unordered_set<Cm::Sym::Symbol*> CCSymbols() const
    {
        return ccSymbols;
    }
    Cm::Sym::ClassTypeSymbol* ContainingClass() const
    {
        return containingClass;
    }
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* containerScope;
    Cm::Sym::FileScope* fileScope;
    std::stack<Cm::Sym::ContainerScope*> containerScopeStack;
    std::unordered_set<Cm::Sym::Symbol*> ccSymbols;
    Cm::Sym::ScopeLookup lookup;
    std::stack<Cm::Sym::ScopeLookup> lookupStack;
    Cm::Sym::SymbolTypeSetId typeSet;
    std::stack<Cm::Sym::SymbolTypeSetId> typeSetStack;
    bool collectFileScope;
    std::stack<Cm::Sym::Symbol*> symbolStack;
    std::stack<bool> collectFileScopeStack;
    Cm::Sym::ClassTypeSymbol* currentClassSymbol;
    Cm::Sym::ClassTypeSymbol* containingClass;
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    bool includeConceptSymbols;
    std::stack<bool> includeConceptSymbolsStack;
    void PushLookup(Cm::Sym::ScopeLookup lookup_)
    {
        lookupStack.push(lookup);
        lookup = lookup_;
    }
    void PopLookup()
    {
        lookup = lookupStack.top();
        lookupStack.pop();
    }
    void PushTypeSet(Cm::Sym::SymbolTypeSetId typeSet_)
    {
        typeSetStack.push(typeSet);
        typeSet = typeSet_;
    }
    void PopTypeSet()
    {
        typeSet = typeSetStack.top();
        typeSetStack.pop();
    }
    void PushCollectFileScope(bool collectFileScope_)
    {
        collectFileScopeStack.push(collectFileScope);
        collectFileScope = collectFileScope_;
    }
    void PopCollectFileScope()
    {
        collectFileScope = collectFileScopeStack.top();
        collectFileScopeStack.pop();
    }
    void BeginContainerScope(Cm::Sym::ContainerScope* containerScope_)
    {
        containerScopeStack.push(containerScope);
        containerScope = containerScope_;
    }
    void EndContainerScope()
    {
        containerScope = containerScopeStack.top();
        containerScopeStack.pop();
    }
    void Collect()
    {
        if (currentClassSymbol)
        {
            containingClass = currentClassSymbol;
        }
        containerScope->CollectSymbolsForCC(ccSymbols, lookup, typeSet, includeConceptSymbols);
        if (collectFileScope)
        {
            fileScope->CollectSymbolsForCC(ccSymbols, typeSet, includeConceptSymbols);
        }
    }
};

CodeCompletionVisitor::CodeCompletionVisitor(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::FileScope* fileScope_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_) : 
    Cm::Ast::Visitor(true, true), symbolTable(symbolTable_), fileScope(fileScope_), lookup(Cm::Sym::ScopeLookup::this_and_base_and_parent), 
    typeSet(Cm::Sym::SymbolTypeSetId::lookupAllSymbols), collectFileScope(true), boundCompileUnit(boundCompileUnit_), currentClassSymbol(nullptr), containingClass(nullptr), includeConceptSymbols(false)
{
}

class CCKey
{
public:
    CCKey(char tag_, const std::string& name_) : tag(tag_), name(name_)
    {
    }
    char Tag() const { return tag; }
    const std::string& Name() const { return name; }
private:
    char tag;
    std::string name;
};

bool operator<(const CCKey& left, const CCKey& right) 
{
    std::string leftName = Cm::Util::ToLower(left.Name());
    std::string rightName = Cm::Util::ToLower(right.Name());
    if (leftName < rightName)
    {
        return true;
    }
    else if (leftName > rightName)
    {
        return false;
    }
    else if (left.Tag()< right.Tag())
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct CompareLower
{
    bool operator()(const std::string& left, const std::string& right) const
    {
        std::string l = Cm::Util::ToLower(left);
        std::string r = Cm::Util::ToLower(right);
        return l < r;
    }
};

void DoCodeCompletion(Cm::Ast::CompileUnitNode& ccUnit, Cm::Sym::SymbolTable& symbolTable, Cm::Sym::FileScope* fileScope, const std::string& ccResultFilePath, 
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit)
{
    CodeCompletionVisitor visitor(symbolTable, fileScope, boundCompileUnit);
    ccUnit.Accept(visitor);
    const std::unordered_set<Cm::Sym::Symbol*>& ccSymbols = visitor.CCSymbols();
    std::map<CCKey, std::vector<Cm::Sym::Symbol*>> ccMap;
    for (Cm::Sym::Symbol* symbol : ccSymbols)
    {
        CCKey ccKey(symbol->CCTag(), symbol->CCName());
        std::vector<Cm::Sym::Symbol*>& ccList = ccMap[ccKey];
        ccList.push_back(symbol);
    }
    std::ofstream ccResultFile(ccResultFilePath);
    std::map<CCKey, std::vector<Cm::Sym::Symbol*>>::const_iterator e = ccMap.cend();
    for (std::map<CCKey, std::vector<Cm::Sym::Symbol*>>::const_iterator i = ccMap.cbegin(); i != e; ++i)
    {
        const CCKey& ccKey = i->first;
        const std::vector<Cm::Sym::Symbol*>& ccList = i->second;
        std::vector<std::string> list;
        for (Cm::Sym::Symbol* symbol : ccList)
        {
            if (symbol->Access() == Cm::Sym::SymbolAccess::private_)
            {
                Cm::Sym::ClassTypeSymbol* containingClass = symbol->Class();
                if (containingClass)
                {
                    if (containingClass != visitor.ContainingClass())
                    {
                        continue;
                    }
                }
            }
            std::string s = symbol->FullCCName(symbolTable);
            if (!s.empty())
            {
                list.push_back(s);
            }
        }
        if (!list.empty())
        {
            ccResultFile << std::string(1, ccKey.Tag()) << ":" << ccKey.Name() << "\n[\n";
            std::sort(list.begin(), list.end(), CompareLower());
            for (const std::string& s : list)
            {
                ccResultFile << "    " << s << ";\n";
            }
            ccResultFile << "]\n";
        }
    }
}

} } // namespace Cm::Bind
