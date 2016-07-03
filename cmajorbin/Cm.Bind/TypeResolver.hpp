/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_TYPE_RESOLVER_INCLUDED
#define CM_BIND_TYPE_RESOLVER_INCLUDED
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Visitor.hpp>

namespace Cm { namespace Bind {

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


enum class TypeResolverFlags: uint8_t
{
    none = 0,
    dontThrow = 1 << 0
};

inline TypeResolverFlags operator&(TypeResolverFlags left, TypeResolverFlags right)
{
    return TypeResolverFlags(uint8_t(left) & uint8_t(right));
}

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::Node* typeExpr);

Cm::Sym::TypeSymbol* ResolveType(Cm::Sym::SymbolTable& symbolTable, Cm::Sym::ContainerScope* currentContainerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
    Cm::Core::ClassTemplateRepository& classTemplateRepository, Cm::BoundTree::BoundCompileUnit& boundCompileUnit, Cm::Ast::Node* typeExpr, TypeResolverFlags flags);

} } // namespace Cm::Bind

#endif // CM_BIND_TYPE_RESOLVER_INCLUDED
