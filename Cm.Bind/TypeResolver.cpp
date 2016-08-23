/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Core/Exception.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Bind/Evaluator.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/TypeParameterSymbol.hpp>
#include <Cm.Sym/GlobalFlags.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Expression.hpp>

namespace Cm { namespace Bind {

class TypeResolver : public Cm::Ast::Visitor
{
public:
    TypeResolver(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
        Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, TypeResolverFlags flags_);
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
    void Visit(Cm::Ast::WCharNode& wcharNode) override;
    void Visit(Cm::Ast::UCharNode& ucharNode) override;
    void Visit(Cm::Ast::VoidNode& voidNode) override;
    void Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode) override;
    void Visit(Cm::Ast::TemplateIdNode& templateIdNode) override;
    void Visit(Cm::Ast::IdentifierNode& identifierNode) override;
    void BeginVisit(Cm::Ast::DotNode& dotNode) override;
    void EndVisit(Cm::Ast::DotNode& dotNode) override;
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* currentContainerScope;
    const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes;
    Cm::Core::ClassTemplateRepository& classTemplateRepository;
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    TypeResolverFlags flags;
    Cm::Sym::TypeSymbol* typeSymbol;
    std::unique_ptr<Cm::Sym::TypeSymbol> nsTypeSymbol;
    Cm::Sym::SymbolTypeSetId lookupId;
    Cm::Sym::LookupIdStack lookupIdStack;
    void ResolveSymbol(Cm::Ast::Node* node, Cm::Sym::Symbol* symbol);
};

TypeResolver::TypeResolver(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes_, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository_, Cm::BoundTree::BoundCompileUnit& boundCompileUnit_, TypeResolverFlags flags_) :
    Cm::Ast::Visitor(true, true), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScopes(fileScopes_), classTemplateRepository(classTemplateRepository_), 
    boundCompileUnit(boundCompileUnit_), flags(flags_), typeSymbol(nullptr), lookupId(Cm::Sym::SymbolTypeSetId::lookupTypeSymbols)
{
}

Cm::Sym::TypeSymbol* TypeResolver::Resolve(Cm::Ast::Node* typeExpr)
{
    typeExpr->Accept(*this);
    if (typeSymbol && typeSymbol->IsNamespaceTypeSymbol() && (flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
    {
        throw Cm::Core::Exception("symbol '" + typeSymbol->FullName() + "' denotes a namespace (type symbol expected)", typeExpr->GetSpan());
    }
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

void TypeResolver::Visit(Cm::Ast::WCharNode& wcharNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::wcharId));
}

void TypeResolver::Visit(Cm::Ast::UCharNode& ucharNode)
{
    typeSymbol = symbolTable.GetTypeRepository().GetType(Cm::Sym::GetBasicTypeId(Cm::Sym::ShortBasicTypeId::ucharId));
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
    Cm::Sym::TypeSymbol* subjectType = ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, templateIdNode.Subject(), flags);
    if (!subjectType)
    {
        typeSymbol = nullptr;
        return;
    }
    if (subjectType->IsNamespaceTypeSymbol() && (flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
    {
        throw Cm::Core::Exception("symbol '" + subjectType->FullName() + "' denotes a namespace (type symbol expected)", templateIdNode.GetSpan());
    }
    for (const std::unique_ptr<Cm::Ast::Node>& templateArgNode : templateIdNode.TemplateArguments())
    {
        Cm::Sym::TypeSymbol* argumentType = ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, templateArgNode.get(), flags);
        if (!argumentType)
        {
            typeSymbol = nullptr;
            return;
        }
        if (argumentType->IsNamespaceTypeSymbol() && (flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
        {
            throw Cm::Core::Exception("symbol '" + argumentType->FullName() + "' denotes a namespace (type symbol expected)", templateArgNode->GetSpan());
        }
        typeArguments.push_back(argumentType);
    }
    if (subjectType->IsClassTypeSymbol())
    {
        Cm::Sym::ClassTypeSymbol* subjectClassType = static_cast<Cm::Sym::ClassTypeSymbol*>(subjectType);
        int n = int(subjectClassType->TypeParameters().size());
        int m = int(typeArguments.size());
        if (m < n)
        {
            classTemplateRepository.ResolveDefaultTypeArguments(typeArguments, subjectClassType, currentContainerScope, fileScopes, templateIdNode.GetSpan());
        }
    }
    else
    {
        throw Cm::Core::Exception("class type symbol expected", templateIdNode.GetSpan(), subjectType->GetSpan());
    }
    typeSymbol = symbolTable.GetTypeRepository().MakeTemplateType(subjectType, typeArguments, templateIdNode.GetSpan());
}

void TypeResolver::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    Cm::Sym::Symbol* symbol = currentContainerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent, lookupId);
    if (!symbol)
    {
        for (const std::unique_ptr<Cm::Sym::FileScope>& fileScope : fileScopes)
        {
            symbol = fileScope->Lookup(identifierNode.Str(), lookupId);
            if (symbol) break;
        }
    }
    if (symbol)
    {
        ResolveSymbol(&identifierNode, symbol);
    }
    else if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
    {
        throw Cm::Core::Exception("type symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
    else
    {
        typeSymbol = nullptr;
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
                BindTypedef(symbolTable, scope, fileScopes, classTemplateRepository, boundCompileUnit, typedefNode, typedefSymbol);
            }
            else if (tn->IsTypedefStatementNode())
            {
                Cm::Ast::TypedefStatementNode* typedefStatementNode = static_cast<Cm::Ast::TypedefStatementNode*>(tn);
                Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(typedefStatementNode);
                BindTypedef(symbolTable, scope, fileScopes, classTemplateRepository, boundCompileUnit, typedefStatementNode, typedefSymbol);
            }
            else if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
            {
                throw std::runtime_error("node is not typedef node or typedef statement node");
            }
            else
            {
                typeSymbol = nullptr; 
                return;
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
    else if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
    {
        throw Cm::Core::Exception("symbol '" + symbol->FullName() + "' does not denote a type", node->GetSpan(), symbol->GetSpan());
    }
    else
    {
        typeSymbol = nullptr;
    }
}

void TypeResolver::BeginVisit(Cm::Ast::DotNode& dotNode)
{
    lookupIdStack.Push(lookupId);
    lookupId = Cm::Sym::SymbolTypeSetId::lookupClassAndNamespaceSymbols;
}

void TypeResolver::EndVisit(Cm::Ast::DotNode& dotNode)
{
    lookupId = lookupIdStack.Pop();
    if (!typeSymbol)
    {
        return;
    }
    bool generateDocs = Cm::Sym::GetGlobalFlag(Cm::Sym::GlobalFlags::generate_docs);
    if (typeSymbol->IsClassTypeSymbol() || typeSymbol->IsNamespaceTypeSymbol() || generateDocs && typeSymbol->IsTypeParameterSymbol())
    {
        if (typeSymbol->IsTemplateTypeSymbol())
        {
            if (!typeSymbol->Bound())
            {
                Cm::Sym::TemplateTypeSymbol* templateTypeSymbol = static_cast<Cm::Sym::TemplateTypeSymbol*>(typeSymbol);
                classTemplateRepository.BindTemplateTypeSymbol(templateTypeSymbol, currentContainerScope, fileScopes);
            }
        }
        Cm::Sym::Scope* containerScope = nullptr;
        if (typeSymbol->IsClassTypeSymbol() || generateDocs && typeSymbol->IsTypeParameterSymbol())
        {
            containerScope = typeSymbol->GetContainerScope();
        }
        else
        {
            NamespaceTypeSymbol* nsTypeSymbol = static_cast<NamespaceTypeSymbol*>(typeSymbol);
            containerScope = nsTypeSymbol->Ns()->GetContainerScope();
        }
        const std::string& memberName = dotNode.MemberStr();
        Cm::Sym::Symbol* symbol = containerScope->Lookup(memberName, Cm::Sym::ScopeLookup::this_and_base, lookupId);
        if (symbol)
        {
            ResolveSymbol(&dotNode, symbol);
        }
        else if (generateDocs && typeSymbol->IsTypeParameterSymbol())
        {
            symbol = new Cm::Sym::TypeParameterSymbol(dotNode.GetSpan(), memberName);
            typeSymbol->AddSymbol(symbol);
            typeSymbol = static_cast<Cm::Sym::TypeSymbol*>(symbol);
        }
        else if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
        {
            throw Cm::Core::Exception("type symbol '" + memberName + "' not found", dotNode.GetSpan());
        }
        else
        {
            typeSymbol = nullptr;
        }
    }
    else if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
    {
        throw Cm::Core::Exception("symbol '" + typeSymbol->FullName() + "' does not denote a class or a namespace", dotNode.GetSpan(), typeSymbol->GetSpan());
    }
    else
    {
        typeSymbol = nullptr;
    }
}

void TypeResolver::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    Cm::Sym::TypeSymbol* baseType = ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, derivedTypeExprNode.BaseTypeExprNode(), flags);
    if (!baseType)
    {
        typeSymbol = nullptr;
        return;
    }
    std::vector<int> arrayDimensions;
    int n = derivedTypeExprNode.NumArrayDimensions();
    if (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            Cm::Sym::Value* value = Evaluate(Cm::Sym::ValueType::intValue, false, derivedTypeExprNode.ArrayDimensionNode(i), symbolTable, currentContainerScope, fileScopes, classTemplateRepository, 
                boundCompileUnit);
            Cm::Sym::IntValue* intValue = static_cast<Cm::Sym::IntValue*>(value);
            int arrayDimension = intValue->Value();
            if (arrayDimension <= 0)
            {
                if ((flags & TypeResolverFlags::dontThrow) == TypeResolverFlags::none)
                {
                    throw Cm::Core::Exception("array dimension must be positive", derivedTypeExprNode.GetSpan());
                }
                else
                {
                    typeSymbol = nullptr;
                    return;
                }
            }
            arrayDimensions.push_back(arrayDimension);
        }
    }
    typeSymbol = symbolTable.GetTypeRepository().MakeDerivedType(derivedTypeExprNode.Derivations(), baseType, arrayDimensions, derivedTypeExprNode.GetSpan());
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::Node* typeExpr)
{
    return ResolveType(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, typeExpr, TypeResolverFlags::none);
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::Node* typeExpr, TypeResolverFlags flags)
{
    TypeResolver resolver(symbolTable, currentContainerScope, fileScopes, classTemplateRepository, boundCompileUnit, flags);
    return resolver.Resolve(typeExpr);
}

} } // namespace Cm::Bind
