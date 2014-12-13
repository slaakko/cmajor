/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Bind/Exception.hpp>
#include <Cm.Bind/Typedef.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/TypedefSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

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
    void Visit(Cm::Ast::DotNode& dotNode) override;
private:
    Cm::Sym::SymbolTable& symbolTable;
    Cm::Sym::ContainerScope* currentContainerScope;
    Cm::Sym::FileScope* fileScope;
    Cm::Sym::TypeSymbol* typeSymbol;
};

TypeResolver::TypeResolver(Cm::Sym::SymbolTable& symbolTable_, Cm::Sym::ContainerScope* currentContainerScope_, Cm::Sym::FileScope* fileScope_) :
    Cm::Ast::Visitor(true), symbolTable(symbolTable_), currentContainerScope(currentContainerScope_), fileScope(fileScope_), typeSymbol(nullptr)
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

void TypeResolver::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    Cm::Sym::Symbol* symbol = currentContainerScope->Lookup(identifierNode.Str(), Cm::Sym::ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        symbol = fileScope->Lookup(identifierNode.Str());
    }
    if (symbol)
    {
        if (symbol->IsTypedefSymbol())
        {
            Cm::Sym::TypedefSymbol* typedefSymbol = static_cast<Cm::Sym::TypedefSymbol*>(symbol);
            if (!typedefSymbol->Bound())
            {
                Cm::Ast::Node* node = symbolTable.GetNode(typedefSymbol);
                if (node->IsTypedefNode())
                {
                    Cm::Ast::TypedefNode* typedefNode = static_cast<Cm::Ast::TypedefNode*>(node);
                    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(typedefNode);
                    BindTypedef(symbolTable, scope, fileScope, typedefNode);
                }
                else
                {
                    throw std::runtime_error("node is not typedef node");
                }
            }
            symbol = typedefSymbol->GetType();
        }
        if (symbol->IsTypeSymbol())
        {
            typeSymbol = static_cast<Cm::Sym::TypeSymbol*>(symbol);
        }
        else
        {
            throw Exception("symbol '" + symbol->FullName() + "' does not denote a type", symbol->GetSpan());
        }
    }
    else
    {
        throw Exception("symbol '" + identifierNode.Str() + "' not found", identifierNode.GetSpan());
    }
}

void TypeResolver::Visit(Cm::Ast::DotNode& dotNode)
{
    // todo
}

void TypeResolver::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    Cm::Sym::ContainerScope* scope = symbolTable.GetContainerScope(&derivedTypeExprNode);
    Cm::Sym::TypeSymbol* baseType = ResolveType(symbolTable, scope, fileScope, derivedTypeExprNode.BaseTypeExprNode());
    typeSymbol = symbolTable.GetTypeRepository().MakeDerivedType(derivedTypeExprNode.Derivations(), baseType, derivedTypeExprNode.GetSpan());
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::Node* typeExpr)
{
    TypeResolver resolver(symbolTable, currentContainerScope, fileScope);
    return resolver.Resolve(typeExpr);
}

} } // namespace Cm::Bind
