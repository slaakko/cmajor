/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef IR_INTF_OBJECT_INCLUDED
#define IR_INTF_OBJECT_INCLUDED
#include <string>
#include <vector>
#include <memory>

namespace Ir { namespace Intf {

class Object;
class Emitter;
class Constant;
class Global;
class RegVar;
class StackVar;
class MemberVar;
class RefVar;
class Parameter;
class Type;

class Object
{
public:
    Object(const std::string& name_, Type* type_);
    virtual ~Object();
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_) { name = name_; }
    Type* GetType() const { return type; }
    void SetType(Type* type_);
    virtual void InitTo(Emitter& emitter, Type* type, Object* to) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, Constant& constant) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, Global& global) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, RegVar& regVar) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, StackVar& stackVar) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, MemberVar& memberVar) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, RefVar& refVar) = 0;
    virtual void InitFrom(Emitter& emitter, Type* type, Parameter& parameter) = 0;
    virtual void AssignTo(Emitter& emitter, Type* type, Object* to) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, Constant& constant) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, Global& global) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, RegVar& regVar) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, StackVar& stackVar) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, MemberVar& memberVar) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, RefVar& refVar) = 0;
    virtual void AssignFrom(Emitter& emitter, Type* type, Parameter& parameter) = 0;
    virtual Object* CreateAddr(Emitter& emitter, Type* type_);
    virtual bool IsNull() const { return false; }
    virtual bool IsConstant() const { return false; }
    virtual bool IsRegVar() const { return false; }
    virtual bool IsStackVar() const { return false; }
    virtual bool IsGlobal() const { return false; }
    virtual bool IsParameter() const { return false; }
    bool Owned() const { return owned; }
    void SetOwned() { owned = true; }
private:
    std::string name;
    Type* type;
    std::unique_ptr<Type> ownedType;
    bool owned;
};

} } // namespace Ir::Intf

#endif // IR_INTF_OBJECT_INCLUDED
