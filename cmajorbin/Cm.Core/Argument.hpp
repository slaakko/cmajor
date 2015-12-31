/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_ARGUMENT_INCLUDED
#define CM_CORE_ARGUMENT_INCLUDED
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Core {

enum class ArgumentCategory : uint8_t
{
    rvalue, lvalue
};

class Argument
{
public:
    Argument(ArgumentCategory category_, Cm::Sym::TypeSymbol* type_);
    ArgumentCategory Category() const { return category; }
    Cm::Sym::TypeSymbol* Type() const { return type; }
    void SetType(Cm::Sym::TypeSymbol* type_) { type = type_; }
    bool BindToRvalueRef() const { return bindToRvalueRef; }
    void SetBindToRvalueRef() { bindToRvalueRef = true; }
private:
    ArgumentCategory category;
    Cm::Sym::TypeSymbol* type;
    bool bindToRvalueRef;
};

} } // namespace Cm::Core

#endif // CM_CORE_ARGUMENT_INCLUDED
