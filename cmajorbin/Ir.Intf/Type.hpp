/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef IR_INTF_TYPE_INCLUDED
#define IR_INTF_TYPE_INCLUDED
#include <string>

namespace Ir { namespace Intf {

class Object;

class Type
{
public:
    Type(const std::string& name_);
    virtual ~Type();
    virtual Type* Clone() const = 0;
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_) { name = name_; }
    virtual bool IsIntegerType() const { return false; }
    virtual bool IsFloatingPointType() const { return false; }
    virtual bool IsPointerType() const { return false; }
    virtual bool IsRvalueRefType() const { return false; }
    virtual bool IsVoidType() const { return false; }
    virtual bool IsFunctionType() const { return false; }
    virtual bool IsFunctionPointerType() const { return false; }
    virtual bool IsFunctionPtrPtrType() const { return false; }
    virtual bool IsTypeName() const { return false; }
    virtual std::string Prefix() const { return Name() + " "; }
    virtual std::string Suffix() const { return std::string(); }
    virtual const std::string& GetTagName() const { return name; }
    virtual Object* CreateDefaultValue() const;
    virtual Object* CreateMinusOne() const;
    virtual Object* CreatePlusOne() const;
    virtual uint8_t NumPointers() const { return 0; }
    bool Owned() const { return owned; }
    void SetOwned() { owned = true; }
private:
    std::string name;
    bool owned;
};

} } // namespace Ir::Intf

#endif // IR_INTF_TYPE_INCLUDED
