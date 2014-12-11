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
#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Sym/MemberVariableSymbol.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

SymbolTable::SymbolTable() : globalNs(Span(), ""), container(&globalNs)
{
    BoolTypeSymbol* boolTypeSymbol = new BoolTypeSymbol();
    globalNs.AddSymbol(boolTypeSymbol);
    AddType(boolTypeSymbol);
    CharTypeSymbol* charTypeSymbol = new CharTypeSymbol();
    globalNs.AddSymbol(charTypeSymbol);
    AddType(charTypeSymbol);
    VoidTypeSymbol* voidTypeSymbol = new VoidTypeSymbol();
    globalNs.AddSymbol(voidTypeSymbol);
    AddType(voidTypeSymbol);
    SByteTypeSymbol* sbyteTypeSymbol = new SByteTypeSymbol();
    globalNs.AddSymbol(sbyteTypeSymbol);
    AddType(sbyteTypeSymbol);
    ByteTypeSymbol* byteTypeSymbol = new ByteTypeSymbol();
    globalNs.AddSymbol(byteTypeSymbol);
    AddType(byteTypeSymbol);
    ShortTypeSymbol* shortTypeSymbol = new ShortTypeSymbol();
    globalNs.AddSymbol(shortTypeSymbol);
    AddType(shortTypeSymbol);
    UShortTypeSymbol* ushortTypeSymbol = new UShortTypeSymbol();
    globalNs.AddSymbol(ushortTypeSymbol);
    AddType(ushortTypeSymbol);
    IntTypeSymbol* intTypeSymbol = new IntTypeSymbol();
    globalNs.AddSymbol(intTypeSymbol);
    AddType(intTypeSymbol);
    UIntTypeSymbol* uintTypeSymbol = new UIntTypeSymbol();
    globalNs.AddSymbol(uintTypeSymbol);
    AddType(uintTypeSymbol);
    LongTypeSymbol* longTypeSymbol = new LongTypeSymbol();
    globalNs.AddSymbol(longTypeSymbol);
    AddType(longTypeSymbol);
    ULongTypeSymbol* ulongTypeSymbol = new ULongTypeSymbol();
    globalNs.AddSymbol(ulongTypeSymbol);
    AddType(ulongTypeSymbol);
    FloatTypeSymbol* floatTypeSymbol = new FloatTypeSymbol();
    globalNs.AddSymbol(floatTypeSymbol);
    AddType(floatTypeSymbol);
    DoubleTypeSymbol* doubleTypeSymbol = new DoubleTypeSymbol();
    globalNs.AddSymbol(doubleTypeSymbol);
    AddType(doubleTypeSymbol);
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
    AddType(classSymbol);
    ContainerScope* classScope = classSymbol->GetContainerScope();
    nodeScopeMap[classNode] = classScope;
    symbolNodeMap[classSymbol] = classNode;
    ContainerScope* containerScope = container->GetContainerScope();
    classScope->SetParent(containerScope);
    container->AddSymbol(classSymbol);
    BeginContainer(classSymbol);
}

void SymbolTable::EndClassScope()
{
    EndContainer();
}

void SymbolTable::BeginEnumScope(Cm::Ast::EnumTypeNode* enumTypeNode)
{
    Cm::Ast::IdentifierNode* enumTypeId = enumTypeNode->Id();
    EnumTypeSymbol* enumTypeSymbol = new EnumTypeSymbol(enumTypeId->GetSpan(), enumTypeId->Str());
    AddType(enumTypeSymbol);
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
    container->AddSymbol(new EnumConstantSymbol(enumConstantId->GetSpan(), enumConstantId->Str()));
}

void SymbolTable::AddTypedef(Cm::Ast::TypedefNode* typedefNode)
{
    Cm::Ast::IdentifierNode* typedefId = typedefNode->Id();
    container->AddSymbol(new TypedefSymbol(typedefId->GetSpan(), typedefId->Str()));
}

void SymbolTable::BeginFunctionScope(Cm::Ast::FunctionNode* functionNode)
{
    FunctionSymbol* functionSymbol = new FunctionSymbol(functionNode->GetSpan(), functionNode->Name());
    ContainerScope* functionScope = functionSymbol->GetContainerScope();
    nodeScopeMap[functionNode] = functionScope;
    symbolNodeMap[functionSymbol] = functionNode;
    ContainerScope* containerScope = container->GetContainerScope();
    functionScope->SetParent(containerScope);
    container->AddFunctionSymbol(functionSymbol);
    BeginContainer(functionSymbol);
}

void SymbolTable::EndFunctionScope()
{
    EndContainer();
}

void SymbolTable::BeginDelegateScope(Cm::Ast::DelegateNode* delegateNode)
{
    Cm::Ast::IdentifierNode* delegateId = delegateNode->Id();
    DelegateSymbol* delegateSymbol = new DelegateSymbol(delegateId->GetSpan(), delegateId->Str());
    AddType(delegateSymbol);
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
    ClassDelegateSymbol* classDelegateSymbol = new ClassDelegateSymbol(classDelegateId->GetSpan(), classDelegateId->Str());
    AddType(classDelegateSymbol);
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

void SymbolTable::AddParameter(Cm::Ast::ParameterNode* parameterNode, const std::string& parameterName)
{
    ParameterSymbol* parameterSymbol = new ParameterSymbol(parameterNode->GetSpan(), parameterName);
    container->AddSymbol(parameterSymbol);
    symbolNodeMap[parameterSymbol] = parameterNode;
}

void SymbolTable::AddTemplateParameter(Cm::Ast::TemplateParameterNode* templateParameterNode)
{
    Cm::Ast::IdentifierNode* templateParameterId = templateParameterNode->Id();
    TemplateParameterSymbol* templateParameterSymbol = new TemplateParameterSymbol(templateParameterId->GetSpan(), templateParameterId->Str());
    container->AddSymbol(templateParameterSymbol);
    symbolNodeMap[templateParameterSymbol] = templateParameterNode;
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

void SymbolTable::AddMemberVariable(Cm::Ast::MemberVariableNode* memberVariableNode)
{
    Cm::Ast::IdentifierNode* memberVariableId = memberVariableNode->Id();
    MemberVariableSymbol* memberVariableSymbol = new MemberVariableSymbol(memberVariableId->GetSpan(), memberVariableId->Str());
    container->AddSymbol(memberVariableSymbol);
    symbolNodeMap[memberVariableSymbol] = memberVariableNode;
}

void SymbolTable::AddType(TypeSymbol* type)
{
    typeSymbolMap[type->Id()] = type;
}

TypeSymbol* SymbolTable::GetTypeNothrow(const TypeId& typeId) const
{
    TypeSymbolMapIt i = typeSymbolMap.find(typeId);
    if (i != typeSymbolMap.end())
    {
        return i->second;
    }
    else
    {
        return nullptr;
    }
}

TypeSymbol* SymbolTable::GetType(const TypeId& typeId) const
{
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    else
    {
        throw std::runtime_error("type symbol not found");
    }
}

std::string MakeDerivedTypeName(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType)
{
    return Cm::Ast::MakeDerivedTypeName(derivations, baseType->FullName());
}

TypeSymbol* SymbolTable::GetDerivedType(const Cm::Ast::DerivationList& derivations, TypeSymbol* baseType, const Span& span)
{
    TypeId typeId(baseType->Id().BaseTypeId(), derivations);
    TypeSymbol* typeSymbol = GetTypeNothrow(typeId);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    TypeSymbol* derivedTypeSymbol = new TypeSymbol(span, MakeDerivedTypeName(derivations, baseType), typeId);
    AddType(derivedTypeSymbol);
    derivedTypes.push_back(std::unique_ptr<TypeSymbol>(derivedTypeSymbol));
    return derivedTypeSymbol;
}

ContainerScope* SymbolTable::GetContainerScope(Cm::Ast::Node* node) const
{
    NodeScopeMapIt i = nodeScopeMap.find(node);
    if (i != nodeScopeMap.end())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("container scope not found");
    }
}

Cm::Ast::Node* SymbolTable::GetNode(Symbol* symbol) const
{
    SymbolNodeMapIt i = symbolNodeMap.find(symbol);
    if (i != symbolNodeMap.end())
    {
        return i->second;
    }
    else
    {
        throw std::runtime_error("node for symbol not found");
    }
}

void SymbolTable::Export(Writer& writer)
{
    writer.Write(&globalNs);
    std::vector<TypeSymbol*> exportedDerivedTypes;
    for (const std::unique_ptr<TypeSymbol>& derivedType : derivedTypes)
    {
        if (derivedType->IsExportSymbol())
        {
            exportedDerivedTypes.push_back(derivedType.get());
        }
    }
    int32_t n = int32_t(exportedDerivedTypes.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        TypeSymbol* type = exportedDerivedTypes[i];
        writer.Write(type);
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
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        if (symbol->IsTypeSymbol())
        {
            TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(symbol);
            derivedTypes.push_back(std::unique_ptr<TypeSymbol>(typeSymbol));
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
}

} } // namespace Cm::Sym
