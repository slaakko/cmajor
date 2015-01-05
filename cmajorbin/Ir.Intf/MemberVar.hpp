/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_MEMBERVAR_INCLUDED
#define IR_INTF_MEMBERVAR_INCLUDED
#include <Ir.Intf/Object.hpp>

namespace Ir { namespace Intf {

class MemberVar: public Object
{
public:
    MemberVar(const std::string& name_, Object* ptr_, int index_, Type* type_);
    Object* Ptr() const { return ptr; }
    int Index() const { return index; }
    virtual void SetDotMember() {}
private:
    Object* ptr;
    int index;
};

} } // namespace Ir::Intf

#endif // IR_INTF_MEMBERVAR_INCLUDED
