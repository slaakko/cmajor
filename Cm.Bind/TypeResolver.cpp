/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/TypeResolver.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/DerivedTypeSymbol.hpp>

namespace Cm { namespace Bind {

class TypeResolver : public Cm::Ast::Visitor
{
public:
    TypeResolver(Cm::Sym::ContainerScope* currentContainerScope_, Cm::Sym::FileScope* fileScope_);
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
    Cm::Sym::ContainerScope* currentContainerScope;
    Cm::Sym::FileScope* fileScope;
    std::unique_ptr<Cm::Sym::TypeSymbol> typeSymbol;
};

TypeResolver::TypeResolver(Cm::Sym::ContainerScope* currentContainerScope_, Cm::Sym::FileScope* fileScope_) : Cm::Ast::Visitor(true), currentContainerScope(currentContainerScope_), fileScope(fileScope_)
{
}

Cm::Sym::TypeSymbol* TypeResolver::Resolve(Cm::Ast::Node* typeExpr)
{
    typeExpr->Accept(*this);
    return typeSymbol.release();
}

void TypeResolver::Visit(Cm::Ast::BoolNode& boolNode)
{
    typeSymbol.reset(new Cm::Sym::BoolTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::SByteNode& sbyteNode)
{
    typeSymbol.reset(new Cm::Sym::SByteTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::ByteNode& byteNode)
{
    typeSymbol.reset(new Cm::Sym::ByteTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::ShortNode& shortNode)
{
    typeSymbol.reset(new Cm::Sym::ShortTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::UShortNode& shortNode)
{
    typeSymbol.reset(new Cm::Sym::UShortTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::IntNode& intNode)
{
    typeSymbol.reset(new Cm::Sym::IntTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::UIntNode& uintNode)
{
    typeSymbol.reset(new Cm::Sym::UIntTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::LongNode& longNode)
{
    typeSymbol.reset(new Cm::Sym::LongTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::ULongNode& ulongNode) 
{
    typeSymbol.reset(new Cm::Sym::ULongTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::FloatNode& floatNode)
{
    typeSymbol.reset(new Cm::Sym::FloatTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::DoubleNode& doubleNode)
{
    typeSymbol.reset(new Cm::Sym::DoubleTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::CharNode& charNode)
{
    typeSymbol.reset(new Cm::Sym::CharTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::VoidNode& voidNode) 
{
    typeSymbol.reset(new Cm::Sym::VoidTypeSymbol());
}

void TypeResolver::Visit(Cm::Ast::TemplateIdNode& templateIdNode)
{
    // todo
}

void TypeResolver::Visit(Cm::Ast::IdentifierNode& identifierNode)
{
    // todo
}

void TypeResolver::Visit(Cm::Ast::DotNode& dotNode)
{
    // todo
}

void TypeResolver::Visit(Cm::Ast::DerivedTypeExprNode& derivedTypeExprNode)
{
    Cm::Sym::DerivedTypeSymbol* derivedTypeSymbol = new Cm::Sym::DerivedTypeSymbol(derivedTypeExprNode.GetSpan(), derivedTypeExprNode.Name());
    derivedTypeSymbol->SetDerivations(derivedTypeExprNode.Derivations());
    derivedTypeSymbol->SetBaseType(ResolveType(currentContainerScope, fileScope, derivedTypeExprNode.BaseTypeExprNode()));
    typeSymbol.reset(derivedTypeSymbol);
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::ContainerScope* currentContainerScope, Cm::Sym::FileScope* fileScope, Cm::Ast::Node* typeExpr)
{
    TypeResolver resolver(currentContainerScope, fileScope);
    return resolver.Resolve(typeExpr);
}

} } // namespace Cm::Bind
