/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_CLASS_SYMBOL_INCLUDED
#define CM_SYM_CLASS_SYMBOL_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Ast/Class.hpp>

namespace Cm { namespace Sym {

class ClassSymbol : public TypeSymbol
{
public:
    ClassSymbol(const Span& span_, const std::string& name_);
    SymbolType GetSymbolType() const override { return SymbolType::classSymbol; }
    bool IsClassSymbol() const override { return true; }
};

} } // namespace Cm::Sym

#endif // CM_SYM_CLASS_SYMBOL_INCLUDED
