/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef IR_INTF_FACTORY_INCLUDED
#define IR_INTF_FACTORY_INCLUDED

namespace Ir { namespace Intf {

class Type;

class Factory
{
public:
    virtual ~Factory();
    virtual Type* GetLabelType() = 0;
    virtual Type* GetMetadataType() = 0;
    virtual Type* GetI1() = 0;
    virtual Type* GetI8() = 0;
    virtual Type* GetUI8() = 0;
    virtual Type* GetI16() = 0;
    virtual Type* GetUI16() = 0;
    virtual Type* GetI32() = 0;
    virtual Type* GetUI32() = 0;
    virtual Type* GetI64() = 0;
    virtual Type* GetUI64() = 0;
    virtual Type* GetFloat() = 0;
    virtual Type* GetDouble() = 0;
    virtual Type* GetVoid() = 0;
};

void SetFactory(Factory* factory);
Factory* GetFactory();

} } // namespace Ir::Intf

#endif // IR_INTF_FACTORY_INCLUDED
