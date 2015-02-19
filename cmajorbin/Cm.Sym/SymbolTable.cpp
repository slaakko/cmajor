/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/DelegateSymbol.hpp>
#include <Cm.Sym/ConstantSymbol.hpp>
#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/EnumSymbol.hpp>
#include <Cm.Sym/DeclarationBlock.hpp>
#include <Cm.Sym/LocalVariableSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/ConceptSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : globalNs(Span(), ""), container(&globalNs), currentClass(nullptr), currentFunction(nullptr), typeRepository(), standardConversionTable(typeRepository)
{
}

void SymbolTable::BeginContainer(ContainerSymbol* container_)
{
    containerStack.push(container);
    container = container_;
}

void SymbolTable::EndContainer()
{
    container = containerStack.top();
    containerStack.pop();
}

void SymbolTable::BeginNamespaceScope(Cm::Ast::NamespaceNode* namespaceNode)
{
    if (namespaceNode->IsGlobalNamespaceNode())
    {
        if (!globalNs.GetSpan().Valid())
        {
            globalNs.SetSpan(namespaceNode->GetSpan());
        }
        BeginContainer(&globalNs);
        nodeScopeMap[namespaceNode] = globalNs.GetContainerScope();
    }
    else
    {
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceNode->Id()->Str());
        if (symbol)
        {
            if (symbol->IsNamespaceSymbol())
            {
                BeginContainer(static_cast<ContainerSymbol*>(symbol));
                nodeScopeMap[namespaceNode] = symbol->GetContainerScope();
                symbolNodeMap[symbol] = namespaceNode;
            }
            else
            {
                throw Exception("symbol '" + symbol->Name() + "' does not denote a namespace", symbol->GetSpan());
            }
        }
        else
        {
            NamespaceSymbol* namespaceSymbol = container->GetContainerScope()->CreateNamespace(namespaceNode->Id()->Str(), namespaceNode->GetSpan());
            BeginContainer(namespaceSymbol);
            nodeScopeMap[namespaceNode] = namespaceSymbol->GetContainerScope();
            symbolNodeMap[namespaceSymbol] = namespaceNode;
        }
    }
}

void SymbolTable::EndNamespaceScope()
{
    EndContainer();
}

void SymbolTable::BeginNamespaceScope(const std::string& namespaceName, const Span& span)
{
    if (namespaceName.empty())
    {
        if (!globalNs.GetSpan().Valid())
        {
            globalNs.SetSpan(span);
        }
        BeginContainer(&globalNs);
    }
    else
    {
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceName);
        if (symbol)
        {
            if (symbol->IsNamespaceSymbol())
            {
                BeginContainer(static_cast<ContainerSymbol*>(symbol));
            }
            else
            {
                throw Exception("symbol '" + symbol->Name() + "' does not denote a namespace", symbol->GetSpan());
            }
        }
        else
        {
            NamespaceSymbol* namespaceSymbol = container->GetContainerScope()->CreateNamespace(namespaceName, span);
            BeginContainer(namespaceSymbol);
        }
    }
}

void SymbolTable::BeginClassScope(Cm::Ast::ClassNode* classNode)
{
    Cm::Ast::IdentifierNode* classId = classNode->Id();
    ClassTypeSymbol* classSymbol = new ClassTypeSymbol(classId->GetSpan(), classId->Str());
    typeRepository.AddType(classSymbol);
    ContainerScope* classScope = classSymbol->GetContainerScope();
    nodeScopeMap[classNode] = classScope;
    symbolNodeMap[classSymbol] = classNode;
    ContainerScope* containerScope = container->GetContainerScope();
    classScope->SetParent(containerScope);
    container->AddSymbol(classSymbol);
    BeginContainer(classSymbol);
    classSymbol->SetIrType(Cm::IrIntf::CreateClassTypeName(classSymbol->FullName()));
    currentClass = classSymbol; 
}

void SymbolTable::EndClassScope()
{
    currentClass = nullptr;
    EndContainer();
}

void SymbolTable::BeginTemplateTypeScope(Cm::Ast::ClassNode* templateClassNode, Cm::Sym::TemplateTypeSymbol* templateTypeSymbol)
{
    ContainerScope* templateTypeScope = templateTypeSymbol->GetContainerScope();
    nodeScopeMap[templateClassNode] = templateTypeScope;
    symbolNodeMap[templateTypeSymbol] = templateClassNode;
    ContainerScope* containerScope = container->GetContainerScope();
    container->AddSymbol(templateTypeSymbol);
    templateTypeScope->SetParent(containerScope);
    BeginContainer(templateTypeSymbol);
    currentClass = templateTypeSymbol;
}

void SymbolTable::EndTemplateTypeScope()
{
    currentClass = nullptr;
    EndContainer();
}

void SymbolTable::BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode)
{
    Cm::Ast::IdentifierNode* enumTypeId = enumTypeNode->Id();
    EnumTypeSymbol* enumTypeSymbol = new EnumTypeSymbol(enumTypeId->GetSpan(), enumTypeId->Str());
    typeRepository.AddType(enumTypeSymbol);
    ContainerScope* enumScope = enumTypeSymbol->GetContainerScope();
    nodeScopeMap[enumTypeNode] = enumScope;
    symbolNodeMap[enumTypeSymbol] = enumTypeNode;
    ContainerScope* containerScope = container->GetContainerScope();
    enumScope->SetParent(containerScope);
    container->AddSymbol(enumTypeSymbol);
    BeginContainer(enumTypeSymbol);
}

void SymbolTable::EndEnumScope()
{
    EndContainer();
}

void SymbolTable::AddEnumConstant(Cm::Ast::EnumConstantNode* enumConstantNode)
{
    Cm::Ast::IdentifierNode* enumConstantId = enumConstantNode->Id();
    EnumConstantSymbol* enumConstantSymbol = new EnumConstantSymbol(enumConstantId->GetSpan(), enumConstantId->Str());
    container->AddSymbol(enumConstantSymbol);
    symbolNodeMap[enumConstantSymbol] = enumConstantNode;
}

void SymbolTable::AddTypedef(Cm::Ast::TypedefNode* typedefNode)
{
    Cm::Ast::IdentifierNode* typedefId = typedefNode->Id();
    TypedefSymbol* typedefSymbol = new TypedefSymbol(typedefId->GetSpan(), typedefId->Str());
    container->AddSymbol(typedefSymbol);
    symbolNodeMap[typedefSymbol] = typedefNode;
}

void SymbolTable::BeginFunctionScope(Cm::Ast::FunctionNode* functionNode, FunctionSymbolFlags flags)
{
    FunctionSymbol* functionSymbol = new FunctionSymbol(functionNode->GetSpan(), functionNode->Name());
    currentFunction = functionSymbol;
    if ((functionNode->GetSpecifiers() & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        functionSymbol->SetStatic();
    }
    functionSymbol->SetFlags(flags);
    functionSymbol->SetCompileUnit(functionNode->GetCompileUnit());
    functionSymbolMap[functionNode] = functionSymbol;
    functionSymbol->SetGroupName(functionNode->GroupId()->Str());
    ContainerScope* functionScope = functionSymbol->GetContainerScope();
    nodeScopeMap[functionNode] = functionScope;
    symbolNodeMap[functionSymbol] = functionNode;
    ContainerScope* containerScope = container->GetContainerScope();
    functionScope->SetParent(containerScope);
    BeginContainer(functionSymbol);
}

void SymbolTable::EndFunctionScope()
{
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(container);
    EndContainer();
    container->AddSymbol(functionSymbol);
    currentFunction = nullptr;
}

void SymbolTable::BeginDelegateScope(Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Ast::IdentifierNode* delegateId = delegateNode->Id();
    DelegateTypeSymbol* delegateSymbol = new DelegateTypeSymbol(delegateId->GetSpan(), delegateId->Str());
    typeRepository.AddType(delegateSymbol);
    ContainerScope* delegateScope = delegateSymbol->GetContainerScope();
    nodeScopeMap[delegateNode] = delegateScope;
    symbolNodeMap[delegateSymbol] = delegateNode;
    ContainerScope* containerScope = container->GetContainerScope();
    delegateScope->SetParent(containerScope);
    container->AddSymbol(delegateSymbol);
    BeginContainer(delegateSymbol);
}

void SymbolTable::EndDelegateScope()
{
    EndContainer();
}

void SymbolTable::BeginClassDelegateScope(Cm::Ast::ClassDelegateNode* classDelegateNode)
{
    Cm::Ast::IdentifierNode* classDelegateId = classDelegateNode->Id();
    ClassDelegateTypeSymbol* classDelegateSymbol = new ClassDelegateTypeSymbol(classDelegateId->GetSpan(), classDelegateId->Str());
    typeRepository.AddType(classDelegateSymbol);
    ContainerScope* classDelegateScope = classDelegateSymbol->GetContainerScope();
    nodeScopeMap[classDelegateNode] = classDelegateScope;
    symbolNodeMap[classDelegateSymbol] = classDelegateNode;
    ContainerScope* containerScope = container->GetContainerScope();
    classDelegateScope->SetParent(containerScope);
    container->AddSymbol(classDelegateSymbol);
    BeginContainer(classDelegateSymbol);
}

void SymbolTable::EndClassDelegateScope()
{
    EndContainer();
}

void SymbolTable::AddConstant(Cm::Ast::ConstantNode* constantNode)
{
    Cm::Ast::IdentifierNode* constantId = constantNode->Id();
    ConstantSymbol* constantSymbol = new ConstantSymbol(constantId->GetSpan(), constantId->Str());
    container->AddSymbol(constantSymbol);
    symbolNodeMap[constantSymbol] = constantNode;
}

void SymbolTable::AddParameter(ParameterSymbol* parameterSymbol)
{
    container->AddSymbol(parameterSymbol);
}

void SymbolTable::AddParameter(Cm::Ast::ParameterNode* parameterNode, const std::string& parameterName)
{
    ParameterSymbol* parameterSymbol = new ParameterSymbol(parameterNode->GetSpan(), parameterName);
    container->AddSymbol(parameterSymbol);
    symbolNodeMap[parameterSymbol] = parameterNode;
}

void SymbolTable::AddTemplateParameter(Cm::Ast::TemplateParameterNode* templateParameterNode)
{
    Cm::Ast::IdentifierNode* templateParameterId = templateParameterNode->Id();
    TypeParameterSymbol* typeParameterSymbol = new TypeParameterSymbol(templateParameterId->GetSpan(), templateParameterId->Str());
    container->AddSymbol(typeParameterSymbol);
    symbolNodeMap[typeParameterSymbol] = templateParameterNode;
}

void SymbolTable::BeginDeclarationScope(Cm::Ast::StatementNode* statementNode)
{
    DeclarationBlock* declarationBlock = new DeclarationBlock(statementNode->GetSpan(), "");
    ContainerScope* declarationBlockScope = declarationBlock->GetContainerScope();
    nodeScopeMap[statementNode] = declarationBlockScope;
    ContainerScope* containerScope = container->GetContainerScope();
    declarationBlockScope->SetParent(containerScope);
    container->AddSymbol(declarationBlock);
    BeginContainer(declarationBlock);
}

void SymbolTable::EndDeclarationcope()
{
    EndContainer();
}

void SymbolTable::AddLocalVariable(Cm::Ast::ConstructionStatementNode* constructionStatementNode)
{
    Cm::Ast::IdentifierNode* localVariableId = constructionStatementNode->Id();
    LocalVariableSymbol* localVariableSymbol = new LocalVariableSymbol(localVariableId->GetSpan(), localVariableId->Str());
    container->AddSymbol(localVariableSymbol);
    symbolNodeMap[localVariableSymbol] = constructionStatementNode;
}

void SymbolTable::AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode, int memberVariableIndex)
{
    Cm::Ast::IdentifierNode* memberVariableId = memberVariableNode->Id();
    MemberVariableSymbol* memberVariableSymbol = new MemberVariableSymbol(memberVariableId->GetSpan(), memberVariableId->Str());
    if ((memberVariableNode->GetSpecifiers() & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        memberVariableSymbol->SetStatic();
    }
    container->AddSymbol(memberVariableSymbol);
    symbolNodeMap[memberVariableSymbol] = memberVariableNode;
}

ConceptSymbol* SymbolTable::BeginConceptScope(Cm::Ast::ConceptNode* conceptNode)
{
    Cm::Ast::IdentifierNode* conceptId = conceptNode->Id();
    std::string conceptName = conceptId->Str();
    conceptName.append(1, '<');
    bool first = true;
    for (const std::unique_ptr<Cm::Ast::Node>& typeParameter : conceptNode->TypeParameters())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            conceptName.append(", ");
        }
        conceptName.append(typeParameter->Name());
    }
    conceptName.append(1, '>');
    ConceptSymbol* conceptSymbol = new ConceptSymbol(conceptId->GetSpan(), conceptName);
    conceptSymbol->SetGroupName(conceptId->Str());
    for (const std::unique_ptr<Cm::Ast::Node>& typeParameter : conceptNode->TypeParameters())
    {
        Cm::Sym::TypeParameterSymbol* typeParameterSymbol = new Cm::Sym::TypeParameterSymbol(typeParameter->GetSpan(), typeParameter->Name());
        conceptSymbol->AddSymbol(typeParameterSymbol);
    }
    ContainerScope* conceptScope = conceptSymbol->GetContainerScope();
    nodeScopeMap[conceptNode] = conceptScope;
    symbolNodeMap[conceptSymbol] = conceptNode;
    ContainerScope* containerScope = container->GetContainerScope();
    conceptScope->SetParent(containerScope);
    container->AddSymbol(conceptSymbol);
    BeginContainer(conceptSymbol);
    return conceptSymbol;
}

void SymbolTable::EndConceptScope()
{
    EndContainer();
}

ContainerScope* SymbolTable::GetContainerScope(Cm::Ast::Node* node) const
{
    NodeScopeMapIt i = nodeScopeMap.find(node);
    if (i != nodeScopeMap.end())
    {
        return i->second;
    }
    else if (node->Parent())
    {
        return GetContainerScope(node->Parent());
    }
    else
    {
        throw std::runtime_error("container scope not found");
    } 
}

Cm::Ast::Node* SymbolTable::GetNode(Symbol* symbol) const
{
    return GetNode(symbol, true);
}

Cm::Ast::Node* SymbolTable::GetNode(Symbol* symbol, bool throw_) const
{
    SymbolNodeMapIt i = symbolNodeMap.find(symbol);
    if (i != symbolNodeMap.end())
    {
        return i->second;
    }
    else if (throw_)
    {
        throw std::runtime_error("node for symbol '" + symbol->FullName() + "' not found in symbol table");
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::SetNode(Symbol* symbol, Cm::Ast::Node* node)
{
    symbolNodeMap[symbol] = node;
}

FunctionSymbol* SymbolTable::GetFunctionSymbol(Cm::Ast::Node* functionNode) const
{
    NodeFunctionSymbolMapIt i = functionSymbolMap.find(functionNode);
    if (i != functionSymbolMap.end())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("function symbol for function node not found");
    }
}

void SymbolTable::Export(Writer& writer)
{
    std::unordered_set<TemplateTypeSymbol*> exportedTemplateTypes;
    std::unordered_set<Symbol*> collected;
    globalNs.CollectExportedTemplateTypes(collected, exportedTemplateTypes);
    std::unordered_set<TypeSymbol*> exportedDerivedTypes;
    for (TemplateTypeSymbol* exportedTemplateType : exportedTemplateTypes)
    {
        exportedTemplateType->CollectExportedDerivedTypes(exportedDerivedTypes);
    }
    globalNs.CollectExportedDerivedTypes(exportedDerivedTypes);
    writer.Write(&globalNs);
    writer.GetBinaryWriter().Write(int(exportedTemplateTypes.size()));
    for (TemplateTypeSymbol* exportedTemplateType : exportedTemplateTypes)
    {
        writer.Write(exportedTemplateType);
    }
    writer.GetBinaryWriter().Write(int(exportedDerivedTypes.size()));
    for (TypeSymbol* exportedDerivedType : exportedDerivedTypes)
    {
        writer.Write(exportedDerivedType);
    }
}

void SymbolTable::Import(Reader& reader)
{
    std::unique_ptr<Symbol> symbol(reader.ReadSymbol());
    if (symbol->IsNamespaceSymbol())
    {
        NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol.get());
        globalNs.Import(ns, *this);
    }
    else
    {
        throw std::runtime_error("namespace symbol expected");
    }
    typeRepository.Import(reader);
}

void SymbolTable::AddPredefinedSymbolToGlobalScope(Symbol* symbol)
{
    symbol->SetBound();
    symbol->SetSource(SymbolSource::library);
    symbol->SetPublic();
    globalNs.AddSymbol(symbol);
    if (symbol->IsTypeSymbol())
    {
        TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(symbol);
        typeRepository.AddType(typeSymbol);
    }
}

void SymbolTable::InitVirtualFunctionTables()
{
    globalNs.InitVirtualFunctionTables();
}

FunctionSymbol* SymbolTable::GetOverload(const std::string& fullOverloadGroupName) const
{
    Symbol* symbol = globalNs.GetContainerScope()->Lookup(fullOverloadGroupName);
    if (symbol)
    {
        if (symbol->IsFunctionGroupSymbol())
        {
            FunctionGroupSymbol* functionGroup = static_cast<FunctionGroupSymbol*>(symbol);
            return functionGroup->GetOverload();
        }
    }
    return nullptr;
}

} } // namespace Cm::Sym
