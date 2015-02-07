/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Expression.hpp>

namespace Cm { namespace Bind {

class TypeResolver : public Cm::Ast::Visitor
{
public:
    TypeResolver(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, Cm::Sym::FileScope* fileScope_);
    Cm::Sym::TypeSymbol* Resolve(Cm::Ast::Node* typeExpr);
    void Visit(Cm::Ast::BoolNode& boolNode) override;
    void Visit(Cm::Ast::SByteNode& sbyteNode) override;
    void Visit(Cm::Ast::ByteNode& byteNode) override;
    void Visit(Cm::Ast::ShortNode& shortNode) override;
    void Visit(Cm::Ast::UShortNode& shortNode) override;
    void Visit(Cm::Ast::IntNode& intNode) override;
    void Visit(Cm::Ast::UIntNode& uintNode) override;
    void Visit(Cm::Ast::LongNode& longNode) override;
    void Visit(Cm::Ast::ULongNode& ulongNode) override;
    void Visit(Cm::Ast::FloatNode& floatNode) override;
    void Visit(Cm::Ast::DoubleNode& doubleNode) override;
    void Visit(Cm::Ast::CharNode& charNode) override;
    void Visit(Cm::Ast::VoidNode& voidNode) override;
    void Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode) override;
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* currentContainerScope;
    Cm::Sym::FileScope* fileScope;
    Cm::Sym::TypeSymbol* typeSymbol;
    std::unique_ptr<Cm::Sym::TypeSymbol> nsTypeSymbol;
    void ResolveSymbol(Cm::Ast::Node* node, Cm::Sym::Symbol* symbol);
};

TypeResolver::TypeResolver(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, Cm::Sym::FileScope* fileScope_) :
    Cm::Ast::Visitor(true, true), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScope(fileScope_), typeSymbol(nullptr)
{
}

Cm::Sym::TypeSymbol* TypeResolver::Resolve(Cm::Ast::Node* typeExpr)
{
    typeExpr->Accept(*this);
    return typeSymbol;
}

void TypeResolver::Visit(Cm::Ast::BoolNode& boolNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::boolId));
}

void TypeResolver::Visit(Cm::Ast::CharNode& charNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::charId));
}

void TypeResolver::Visit(Cm::Ast::VoidNode& voidNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::voidId));
}

void TypeResolver::Visit(Cm::Ast::SByteNode& sbyteNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::sbyteId));
}

void TypeResolver::Visit(Cm::Ast::ByteNode& byteNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::byteId));
}

void TypeResolver::Visit(Cm::Ast::ShortNode& shortNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::shortId));
}

void TypeResolver::Visit(Cm::Ast::UShortNode& shortNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ushortId));
}

void TypeResolver::Visit(Cm::Ast::IntNode& intNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::intId));
}

void TypeResolver::Visit(Cm::Ast::UIntNode& uintNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::uintId));
}

void TypeResolver::Visit(Cm::Ast::LongNode& longNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::longId));
}

void TypeResolver::Visit(Cm::Ast::ULongNode& ulongNode) 
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ulongId));
}

void TypeResolver::Visit(Cm::Ast::FloatNode& floatNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::floatId));
}

void TypeResolver::Visit(Cm::Ast::DoubleNode& doubleNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::doubleId));
}

void TypeResolver::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    std::vector<Cm::Sym::TypeSymbol*> typeArguments;
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&templateIdNode);
    Cm::Sym::TypeSymbol* subjectType = ResolveType(symbolTable, scope, fileScope, templateIdNode.Subject());
    for (const std::unique_ptr<Cm::Ast::Node>& templateArgNode : templateIdNode.TemplateArguments())
    {
        Cm::Sym::TypeSymbol* argumentType = ResolveType(symbolTable, scope, fileScope, templateArgNode.get());
        typeArguments.push_back(argumentType);
    }
    typeSymbol = symbolTable.GetTypeRepository().MakeTemplateType(subjectType, typeArguments, templateIdNode.GetSpan());
}

class NamespaceTypeSymbol : public Cm::Sym::TypeSymbol
{
public:
    NamespaceTypeSymbol(Cm::Sym::NamespaceSymbol* ns_) : Cm::Sym::TypeSymbol(ns_->GetSpan(), ns_->Name()), ns(ns_)
    {
    }
    Cm::Sym::NamespaceSymbol* Ns() const
    {
        return ns;
    }
    bool IsNamespaceTypeSymbol() const override { return true; }
    std::string GetMangleId() const override { return std::string(); }
    Cm::Sym::SymbolType GetSymbolType() const override { return Cm::Sym::SymbolType::namespaceSymbol; }
private:
    Cm::Sym::NamespaceSymbol* ns;
};

void TypeResolver::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    Cm::Sym::Symbol* symbol = currentContainerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        symbol = fileScope->Lookup(identifierNode.Str());
    }
    if (symbol)
    {
        ResolveSymbol(&identifierNode, symbol);
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
}

void TypeResolver::ResolveSymbol(Cm::Ast::Node* node, Cm::Sym::Symbol* symbol)
{
    if (symbol->IsTypedefSymbol())
    {
        Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
        if (!typedefSymbol->Bound())
        {
            Cm::Ast::Node* tn = symbolTable.GetNode(typedefSymbol);
            if (tn->IsTypedefNode())
            {
                Cm::Ast::TypedefNode* typedefNode = static_cast<Cm::Ast::TypedefNode*>(tn);
                Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(typedefNode);
                BindTypedef(symbolTable, scope, fileScope, typedefNode, typedefSymbol);
            }
            else
            {
                throw std::runtime_error("node is not typedef node");
            }
        }
        symbol = typedefSymbol->GetType();
    }
    else if (symbol->IsBoundTypeParameterSymbol())
    {
        Cm::Sym::BoundTypeParameterSymbol* boundTemplateParam = static_cast<Cm::Sym::BoundTypeParameterSymbol*>(symbol);
        symbol = boundTemplateParam->GetType();
    }
    if (symbol->IsTypeSymbol())
    {
        typeSymbol = static_cast<Cm::Sym::TypeSymbol*>(symbol);
    }
    else if (symbol->IsNamespaceSymbol())
    {
        nsTypeSymbol.reset(new NamespaceTypeSymbol(static_cast<Cm::Sym::NamespaceSymbol*>(symbol)));
        typeSymbol = nsTypeSymbol.get();
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a type", node->GetSpan(), symbol->GetSpan());
    }
}

void TypeResolver::EndVisit(Cm::Ast::DotNode& dotNode)
{
    if (typeSymbol->IsClassTypeSymbol() || typeSymbol->IsNamespaceTypeSymbol())
    {
        Cm::Sym::Scope* containerScope = nullptr;
        if (typeSymbol->IsClassTypeSymbol())
        {
            containerScope = typeSymbol->GetContainerScope();
        }
        else
        {
            NamespaceTypeSymbol* nsTypeSymbol = static_cast<NamespaceTypeSymbol*>(typeSymbol);
            containerScope = nsTypeSymbol->Ns()->GetContainerScope();
        }
        const std::string& memberName = dotNode.MemberId()->Str();
        Cm::Sym::Symbol* symbol = containerScope->Lookup(memberName);
        if (symbol)
        {
            ResolveSymbol(&dotNode, symbol);
        }
        else
        {
            throw Cm::Core::Exception("symbol '" + memberName + "' not found", dotNode.GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("symbol '" + typeSymbol->FullName() + "' does not denote a class or a namespace", dotNode.GetSpan(), typeSymbol->GetSpan());
    }
}

void TypeResolver::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    //Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&derivedTypeExprNode);
    //Cm::Sym::TypeSymbol* baseType = ResolveType(symbolTable, scope, fileScope, derivedTypeExprNode.BaseTypeExprNode());
    Cm::Sym::TypeSymbol* baseType = ResolveType(symbolTable, currentContainerScope, fileScope, derivedTypeExprNode.BaseTypeExprNode());
    typeSymbol = symbolTable.GetTypeRepository().MakeDerivedType(derivedTypeExprNode.Derivations(), baseType, derivedTypeExprNode.GetSpan());
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::Node* typeExpr)
{
    TypeResolver resolver(symbolTable, currentContainerScope, fileScope);
    return resolver.Resolve(typeExpr);
}

} } // namespace Cm::Bind
