/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_ENTRY_SYMBOL_INCLUDED
#define CM_SYM_ENTRY_SYMBOL_INCLUDED
#include <Cm.Sym/VariableSymbol.hpp>

namespace Cm { namespace Sym {

class EntrySymbol : public VariableSymbol
{
public:
    EntrySymbol(const Span& span_);
    EntrySymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::entrySymbol; }
    std::string TypeString() const override { return "entry"; };
    bool IsExportSymbol() const override { return true; }
    bool IsEntrySymbol() const override { return true; }
    bool IsCCSymbol() const override { return false; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_ENTRY_SYMBOL_INCLUDED
