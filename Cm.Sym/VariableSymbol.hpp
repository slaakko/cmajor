/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_VARIABLE_SYMBOL_INCLUDED
#define CM_SYM_VARIABLE_SYMBOL_INCLUDED
#include <Cm.Sym/Symbol.hpp>

namespace Cm { namespace Sym {

class VariableSymbol : public Symbol
{
public:
    VariableSymbol(const Span& span_, const std::string& name_);
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    TypeSymbol* GetType() const override { return type; }
    void SetType(TypeSymbol* type_) { type = type_; }
    void SetType(TypeSymbol* type_, int index) override;
    bool HasType() const { return hasType; }
private:
    TypeSymbol* type;
    bool hasType;
};

} } // namespace Cm::Sym

#endif // CM_SYM_VARIABLE_SYMBOL_INCLUDED
