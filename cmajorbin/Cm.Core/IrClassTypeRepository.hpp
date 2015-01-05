/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
#define CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
#include <Cm.Sym/ClassTypeSymbol.hpp>

namespace Cm { namespace Core {

class IrClassTypeRepository
{
public:
    void AddClassType(Cm::Sym::ClassTypeSymbol* classTypeSymbol);
    void Write(Cm::Util::CodeFormatter& codeFormatter);
private:
    std::unordered_set<Cm::Sym::ClassTypeSymbol*> classTypes;
    void Write(Cm::Sym::ClassTypeSymbol* classType, Cm::Util::CodeFormatter& codeFormatter);
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_CLASS_TYPE_REPOSITORY_INCLUDED
