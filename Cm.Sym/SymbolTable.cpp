/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
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
#include <Cm.Sym/EntrySymbol.hpp>
#include <Cm.Sym/ReturnValueSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/ConceptSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/ClassCounter.hpp>
#include <Cm.Sym/MutexTable.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Sym/InterfaceTypeSymbol.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <iostream>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : nextSid(0), globalNs(Span(), ""), container(&globalNs), currentClass(nullptr), currentFunction(nullptr), typeRepository(*this), standardConversionTable(typeRepository),
    userMainFunction(nullptr)
{
}

SymbolTable::SymbolTable(const SymbolTable& that): globalNs(Span(), ""), typeRepository(*this), standardConversionTable(typeRepository)
{
    globalNs.Import(const_cast<NamespaceSymbol*>(&that.globalNs), *this);
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
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceNode->Id()->Str(), SymbolTypeSetId::lookupNamespace);
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
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceName, SymbolTypeSetId::lookupNamespace);
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

void SymbolTable::BeginClassScope(Cm::Ast::ClassNode* classNode, std::unordered_map<std::string, uint64_t>* cidMap)
{
    Cm::Ast::IdentifierNode* classId = classNode->Id();
    uint64_t cid = noCid;
    if (cidMap)
    {
        std::string classTypeFullName = container->FullName().empty() ? classId->Str() : container->FullName() + "." + classId->Str();
        std::unordered_map<std::string, uint64_t>::const_iterator i = cidMap->find(classTypeFullName);
        if (i != cidMap->cend())
        {
            cid = i->second;
        }
        else
        {
            cid = GetClassCounter()->GetCid();
            (*cidMap)[classTypeFullName] = cid;
        }
    }
    if (cid == noCid)
    {
        cid = GetClassCounter()->GetCid();
    }
    ClassTypeSymbol* classSymbol = new ClassTypeSymbol(classId->GetSpan(), classId->Str(), true, cid);
    projectClasses.insert(classSymbol);
    SetSidAndAddSymbol(classSymbol);
    classSymbol->SetCompileUnit(classNode->GetCompileUnit());
    typeRepository.AddType(classSymbol);
    ContainerScope* classScope = classSymbol->GetContainerScope();
    nodeScopeMap[classNode] = classScope;
    symbolNodeMap[classSymbol] = classNode;
    ContainerScope* containerScope = container->GetContainerScope();
    classScope->SetParent(containerScope);
    container->AddSymbol(classSymbol);
    BeginContainer(classSymbol);
    if (!classSymbol->IrTypeMade())
    {
        classSymbol->SetIrType(Cm::IrIntf::CreateClassTypeName(classSymbol->FullName()));
    }
    currentClassStack.push(currentClass);
    currentClass = classSymbol; 
}

void SymbolTable::EndClassScope()
{
    currentClass = currentClassStack.top();
    currentClassStack.pop();
    EndContainer();
}

void SymbolTable::BeginInterfaceScope(Cm::Ast::InterfaceNode* interfaceNode, std::unordered_map<std::string, uint64_t>* iidMap)
{
    uint64_t iid = noIid;
    if (iidMap)
    {
        std::string interfaceTypeFullName = container->FullName().empty() ? interfaceNode->Id()->Str() : container->FullName() + "." + interfaceNode->Id()->Str();
        std::unordered_map<std::string, uint64_t>::const_iterator i = iidMap->find(interfaceTypeFullName);
        if (i != iidMap->cend())
        {
            iid = i->second;
        }
        else
        {
            iid = GetInterfaceCounter()->GetIid();
            (*iidMap)[interfaceTypeFullName] = iid;
        }
    }
    if (iid == noIid)
    {
        iid = GetInterfaceCounter()->GetIid();
    }
    InterfaceTypeSymbol* interfaceTypeSymbol = new InterfaceTypeSymbol(interfaceNode->GetSpan(), interfaceNode->Id()->Str());
    interfaceTypeSymbol->SetIid(iid);
    interfaceTypeSymbol->SetCompileUnit(interfaceNode->GetCompileUnit());
    ContainerScope* interfaceScope = interfaceTypeSymbol->GetContainerScope();
    nodeScopeMap[interfaceNode] = interfaceScope;
    symbolNodeMap[interfaceTypeSymbol] = interfaceNode;
    ContainerScope* containerScope = container->GetContainerScope();
    interfaceScope->SetParent(containerScope);
    container->AddSymbol(interfaceTypeSymbol);
    BeginContainer(interfaceTypeSymbol);
}

void SymbolTable::EndInterfaceScope()
{
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
    currentClassStack.push(currentClass);
    currentClass = templateTypeSymbol;
}

void SymbolTable::EndTemplateTypeScope()
{
    currentClass = currentClassStack.top();
    EndContainer();
}

void SymbolTable::BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode)
{
    Cm::Ast::IdentifierNode* enumTypeId = enumTypeNode->Id();
    EnumTypeSymbol* enumTypeSymbol = new EnumTypeSymbol(enumTypeId->GetSpan(), enumTypeId->Str());
    SetSidAndAddSymbol(enumTypeSymbol);
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
    SetSidAndAddSymbol(enumConstantSymbol);
    container->AddSymbol(enumConstantSymbol);
    symbolNodeMap[enumConstantSymbol] = enumConstantNode;
}

void SymbolTable::AddTypedef(Cm::Ast::TypedefNode* typedefNode)
{
    Cm::Ast::IdentifierNode* typedefId = typedefNode->Id();
    TypedefSymbol* typedefSymbol = new TypedefSymbol(typedefId->GetSpan(), typedefId->Str());
    SetSidAndAddSymbol(typedefSymbol);
    container->AddSymbol(typedefSymbol);
    symbolNodeMap[typedefSymbol] = typedefNode;
}

void SymbolTable::AddTypedef(Cm::Ast::TypedefStatementNode* typedefStatementNode)
{
    Cm::Ast::IdentifierNode* typedefId = typedefStatementNode->Id();
    TypedefSymbol* typedefSymbol = new TypedefSymbol(typedefId->GetSpan(), typedefId->Str());
    SetSidAndAddSymbol(typedefSymbol);
    container->AddSymbol(typedefSymbol);
    symbolNodeMap[typedefSymbol] = typedefStatementNode;
}

void SymbolTable::BeginFunctionScope(Cm::Ast::FunctionNode* functionNode, FunctionSymbolFlags flags)
{
    FunctionSymbol* functionSymbol = new FunctionSymbol(functionNode->GetSpan(), functionNode->Name());
    SetSidAndAddSymbol(functionSymbol);
    currentFunction = functionSymbol;
    if ((functionNode->GetSpecifiers() & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        functionSymbol->SetStatic();
    }
    functionSymbol->SetFlags(flags);
    functionSymbol->SetCompileUnit(functionNode->GetCompileUnit());
    functionSymbolMap[functionNode] = functionSymbol;
    functionSymbol->SetGroupName(functionNode->GroupId()->Str());
    if (functionSymbol->GroupName() == "main")
    {
        if (userMainFunction)
        {
            throw Cm::Sym::Exception("already has main() function", functionNode->GetSpan(), userMainFunction->GetSpan());
        }
        else
        {
            userMainFunction = functionSymbol;
        }
    }
    ContainerScope* functionScope = functionSymbol->GetContainerScope();
    nodeScopeMap[functionNode] = functionScope;
    symbolNodeMap[functionSymbol] = functionNode;
    ContainerScope* containerScope = container->GetContainerScope();
    functionScope->SetParent(containerScope);
    BeginContainer(functionSymbol);
    AddEntrySymbol();
}

void SymbolTable::EndFunctionScope()
{
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(container);
    EndContainer();
    container->AddSymbol(functionSymbol);
    currentFunction = nullptr;
    if (functionSymbol->IsStaticConstructor())
    {
        functionSymbol->SetMutexId(GetMutexTable()->GetNextMutexId());
    }
}

void SymbolTable::BeginDelegateScope(Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Ast::IdentifierNode* delegateId = delegateNode->Id();
    DelegateTypeSymbol* delegateSymbol = new DelegateTypeSymbol(delegateId->GetSpan(), delegateId->Str());
    SetSidAndAddSymbol(delegateSymbol);
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
    uint64_t nextClassNumber = GetClassCounter()->GetCid();
    ClassDelegateTypeSymbol* classDelegateSymbol = new ClassDelegateTypeSymbol(classDelegateId->GetSpan(), classDelegateId->Str(), true, nextClassNumber);
    SetSidAndAddSymbol(classDelegateSymbol);
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
    SetSidAndAddSymbol(constantSymbol);
    container->AddSymbol(constantSymbol);
    symbolNodeMap[constantSymbol] = constantNode;
}

void SymbolTable::AddParameter(ParameterSymbol* parameterSymbol)
{
    container->AddSymbol(parameterSymbol);
    SetSidAndAddSymbol(parameterSymbol);
}

void SymbolTable::AddParameter(Cm::Ast::ParameterNode* parameterNode, const std::string& parameterName)
{
    ParameterSymbol* parameterSymbol = new ParameterSymbol(parameterNode->GetSpan(), parameterName);
    container->AddSymbol(parameterSymbol);
    SetSidAndAddSymbol(parameterSymbol);
    symbolNodeMap[parameterSymbol] = parameterNode;
}

void SymbolTable::AddTemplateParameter(Cm::Ast::TemplateParameterNode* templateParameterNode)
{
    Cm::Ast::IdentifierNode* templateParameterId = templateParameterNode->Id();
    TypeParameterSymbol* typeParameterSymbol = new TypeParameterSymbol(templateParameterId->GetSpan(), templateParameterId->Str());
    SetSidAndAddSymbol(typeParameterSymbol);
    container->AddSymbol(typeParameterSymbol);
    symbolNodeMap[typeParameterSymbol] = templateParameterNode;
}

void SymbolTable::BeginDeclarationScope(Cm::Ast::StatementNode* statementNode)
{
    DeclarationBlock* declarationBlock = new DeclarationBlock(statementNode->GetSpan(), "@local");
    SetSidAndAddSymbol(declarationBlock);
    ContainerScope* declarationBlockScope = declarationBlock->GetContainerScope();
    nodeScopeMap[statementNode] = declarationBlockScope;
    symbolNodeMap[declarationBlock] = statementNode;
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
    SetSidAndAddSymbol(localVariableSymbol);
    container->AddSymbol(localVariableSymbol);
    symbolNodeMap[localVariableSymbol] = constructionStatementNode;
}

void SymbolTable::AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode)
{
    Cm::Ast::IdentifierNode* memberVariableId = memberVariableNode->Id();
    MemberVariableSymbol* memberVariableSymbol = new MemberVariableSymbol(memberVariableId->GetSpan(), memberVariableId->Str());
    SetSidAndAddSymbol(memberVariableSymbol);
    if ((memberVariableNode->GetSpecifiers() & Cm::Ast::Specifiers::static_) != Cm::Ast::Specifiers::none)
    {
        memberVariableSymbol->SetStatic();
    }
    container->AddSymbol(memberVariableSymbol);
    symbolNodeMap[memberVariableSymbol] = memberVariableNode;
}

void SymbolTable::AddEntrySymbol()
{
    EntrySymbol* entrySymbol = new EntrySymbol(Cm::Parsing::Span());
    SetSidAndAddSymbol(entrySymbol);
    container->AddSymbol(entrySymbol);
}

void SymbolTable::AddReturnValueSymbol(Cm::Ast::Node* returnTypeExprNode)
{
    if (returnTypeExprNode->IsVoidNode()) return;
    ReturnValueSymbol* returnValueSymbol = new ReturnValueSymbol(returnTypeExprNode->GetSpan());
    SetSidAndAddSymbol(returnValueSymbol);
    container->AddSymbol(returnValueSymbol);
    symbolNodeMap[returnValueSymbol] = returnTypeExprNode;
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
    SetSidAndAddSymbol(conceptSymbol);
    if ((conceptNode->GetSpecifiers() & Cm::Ast::Specifiers::public_) != Cm::Ast::Specifiers::none)
    {
        conceptSymbol->SetAccess(SymbolAccess::public_);
    }
    conceptSymbol->SetGroupName(conceptId->Str());
    for (const std::unique_ptr<Cm::Ast::Node>& typeParameter : conceptNode->TypeParameters())
    {
        Cm::Sym::TypeParameterSymbol* typeParameterSymbol = new Cm::Sym::TypeParameterSymbol(typeParameter->GetSpan(), typeParameter->Name());
        SetSidAndAddSymbol(typeParameterSymbol);
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
        FunctionSymbol* functionSymbol = i->second;
        return functionSymbol;
    }
    else
    {
        throw std::runtime_error("function symbol for function node not found");
    }
}

void SymbolTable::Export(Writer& writer)
{
    std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash> exportedTemplateTypes;
    std::unordered_set<Symbol*> collectedTemplateTypes;
    globalNs.CollectExportedTemplateTypes(collectedTemplateTypes, exportedTemplateTypes);
    if (GetGlobalFlag(GlobalFlags::fullConfig))
    {
        typeRepository.CollectExportedTemplateTypes(collectedTemplateTypes, exportedTemplateTypes);
    }
    std::unordered_set<Symbol*> collectedDerivedTypes;
    std::unordered_set<TypeSymbol*> exportedDerivedTypes;
    std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>::const_iterator e = exportedTemplateTypes.cend();
    for (std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>::const_iterator i = exportedTemplateTypes.cbegin(); i != e; ++i)
    {
        const std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes = i->second;
        for (TemplateTypeSymbol* exportedTemplateType : exportedTemplateTypes)
        {
            exportedTemplateType->CollectExportedDerivedTypes(collectedDerivedTypes, exportedDerivedTypes);
        }
    }
    globalNs.CollectExportedDerivedTypes(collectedDerivedTypes, exportedDerivedTypes);
    if (GetGlobalFlag(GlobalFlags::fullConfig))
    {
        typeRepository.CollectExportedDerivedTypes(collectedDerivedTypes, exportedDerivedTypes);
    }
    writer.Write(&globalNs);
    writer.GetBinaryWriter().Write(int(exportedTemplateTypes.size()));
    for (std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>::const_iterator i = exportedTemplateTypes.cbegin(); i != e; ++i)
    {
        TemplateTypeSymbol* exportedTemplateType = *i->second.begin();
        writer.Write(exportedTemplateType);
    }
    writer.GetBinaryWriter().Write(int(exportedDerivedTypes.size()));
    for (TypeSymbol* exportedDerivedType : exportedDerivedTypes)
    {
        writer.Write(exportedDerivedType);
    }
    if (GetGlobalFlag(GlobalFlags::fullConfig))
    {
        writer.GetBinaryWriter().Write(int(exportedTemplateTypes.size()));
        for (std::unordered_map<TypeId, std::unordered_set<TemplateTypeSymbol*>, TypeIdHash>::const_iterator i = exportedTemplateTypes.cbegin(); i != e; ++i)
        {
            const std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes = i->second;
            writer.GetBinaryWriter().Write(int(exportedTemplateTypes.size()));
            for (TemplateTypeSymbol* exportedTemplateType : exportedTemplateTypes)
            {
                writer.GetBinaryWriter().Write(exportedTemplateType->Cid());
            }
        }
    }
}

void SymbolTable::Import(Reader& reader)
{
    Import(reader, true);
}

void SymbolTable::Import(Reader& reader, bool importTypeRepository)
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
    if (importTypeRepository)
    {
        typeRepository.Import(reader, *this);
    }
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
    if (symbol->Sid() == noSid)
    {
        SetSidAndAddSymbol(symbol);
    }
    else
    {
        AddSymbol(symbol);
    }
}

void SymbolTable::InitVirtualFunctionTablesAndInterfaceTables()
{
    globalNs.InitVirtualFunctionTablesAndInterfaceTables();
}

FunctionSymbol* SymbolTable::GetOverload(const std::string& fullOverloadGroupName) const
{
    Symbol* symbol = globalNs.GetContainerScope()->Lookup(fullOverloadGroupName, SymbolTypeSetId::lookupFunctionGroup);
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

void SymbolTable::ProcessImportedTemplateTypes()
{
    if (importedTemplateTypes.empty()) return;
    std::unordered_map<TypeId, std::vector<TemplateTypeSymbol*>, TypeIdHash> replicaMap;
    for (TemplateTypeSymbol* templateTypeSymbol : importedTemplateTypes)
    {
        std::vector<TemplateTypeSymbol*>& replicaList = replicaMap[templateTypeSymbol->Id()];
        replicaList.push_back(templateTypeSymbol);
    }
    std::unordered_map<TypeId, std::vector<TemplateTypeSymbol*>, TypeIdHash>::iterator e = replicaMap.end();
    for (std::unordered_map<TypeId, std::vector<TemplateTypeSymbol*>, TypeIdHash>::iterator i = replicaMap.begin(); i != e; ++i)
    {
        std::vector<TemplateTypeSymbol*>& replicaList = i->second;
        if (!replicaList.empty())
        {
            TemplateTypeSymbol* representative = replicaList.back();
            representative->ResetFlag(SymbolFlags::replica);
            representative->ResetFlag(SymbolFlags::bound);
            int n = int(replicaList.size());
            for (int i = 0; i < n - 1; ++i)
            {
                TemplateTypeSymbol* replica = replicaList[i];
                replica->SetReplica();
                replica->SetBound();
                replica->SetPrimaryTemplateTypeSymbol(representative);
            }
            typeRepository.AddType(representative);
        }
    }
    importedTemplateTypes.clear();
    globalNs.ReplaceReplicaTypes();
    typeRepository.ReplaceReplicaTypes();
}

void SymbolTable::AddSymbol(Symbol* symbol)
{
    if (symbol->Sid() != noSid)
    {
        symbolMap[symbol->Sid()] = symbol;
        if (symbol->IsClassTypeSymbol())
        {
            ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(symbol);
            classes.insert(cls);
            if (cls->Cid() != noCid)
            {
                classMap[cls->Cid()] = cls;
            }
        }
    }
}

uint32_t SymbolTable::GetSid()
{
    return nextSid++;
}

void SymbolTable::SetSidAndAddSymbol(Symbol* symbol)
{
    symbol->SetSid(GetSid());
    AddSymbol(symbol);
}

Symbol* SymbolTable::GetSymbol(uint32_t sid)
{
    std::unordered_map<uint32_t, Symbol*>::const_iterator i = symbolMap.find(sid);
    if (i != symbolMap.cend())
    {
        return i->second;
    }
    return nullptr;
}

ClassTypeSymbol* SymbolTable::GetClass(uint64_t cid) const
{
    std::unordered_map<uint64_t, ClassTypeSymbol*>::const_iterator i = classMap.find(cid);
    if (i != classMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void SymbolTable::Own(LocalVariableSymbol* localVariable)
{
    ownedLocalVariables.push_back(std::unique_ptr<LocalVariableSymbol>(localVariable));
}

const std::unordered_set<ClassTypeSymbol*>& SymbolTable::Classes() const
{
    return classes;
}

const std::unordered_set<ClassTypeSymbol*>& SymbolTable::ProjectClasses() const
{
    return projectClasses;
}

uint32_t SymbolTable::GetVariableSymbolSid(const std::string& variableSymbolFullName)
{
    std::unordered_map<std::string, uint32_t>::const_iterator i = variableSymbolNameSidMap.find(variableSymbolFullName);
    if (i != variableSymbolNameSidMap.cend())
    {
        return i->second;
    }
    uint32_t sid = GetSid();
    variableSymbolNameSidMap[variableSymbolFullName] = sid;
    return sid;
}

void SymbolTable::SetVirtualClassCid(const std::string& className, uint64_t cid)
{
    classNameCidMap[className] = cid;
}

uint64_t SymbolTable::GetVirtualClassCid(const std::string& className) const
{
    std::unordered_map<std::string, uint64_t>::const_iterator i = classNameCidMap.find(className);
    if (i != classNameCidMap.cend())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("cid for class '" + className + "' not found in symbol table");
    }
}

} } // namespace Cm::Sym
