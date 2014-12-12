/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_SYMBOL_INCLUDED
#define CM_SYM_FUNCTION_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Ast/Function.hpp>

namespace Cm { namespace Sym {

class FunctionSymbol : public ContainerSymbol
{
public:
    FunctionSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::functionSymbol; }
    std::string TypeString() const override { return "function"; };
    bool IsExportSymbol() const override { return Source() == SymbolSource::project; }
    bool IsFunctionSymbol() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_FUNCTION_SYMBOL_INCLUDED
