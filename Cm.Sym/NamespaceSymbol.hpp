/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_NAMESPACE_SYMBOL_INCLUDED
#define CM_SYM_NAMESPACE_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>

namespace Cm { namespace Sym {

class NamespaceSymbol : public ContainerSymbol
{
public:
    NamespaceSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::namespaceSymbol; }
    std::string TypeString() const override { return "namespace"; };
    bool IsNamespaceSymbol() const override { return true; }
    bool IsGlobalNamespace() const { return Name().empty(); }
    void Import(NamespaceSymbol* that, SymbolTable& symbolTable);
};

} } // namespace Cm::Sym

#endif // CM_SYM_NAMESPACE_SYMBOL_INCLUDED
