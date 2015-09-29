/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_DECLARATION_BLOCK_INCLUDED
#define CM_SYM_DECLARATION_BLOCK_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Ast/Statement.hpp>

namespace Cm { namespace Sym {

class DeclarationBlock : public ContainerSymbol
{
public:
    DeclarationBlock(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::declarationBlock; }
    std::string TypeString() const override { return "declaration block"; };
    bool IsExportSymbol() const override;
    bool IsDeclarationBlock() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_DECLARATION_BLOCK_INCLUDED
