/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <C.Ir/Constant.hpp>
#include <C.Ir/Type.hpp>
#include <Ir.Intf/Factory.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <stdexcept>

namespace C {

std::string MakeStringConstantName(const std::string& s, bool metadataSyntax)
{
    return "\"" + Cm::Util::StringStr(s) + "\"";
}

std::string MakeStringConstantName(const std::string& s)
{
    return MakeStringConstantName(s, false);
}

Constant::Constant(const std::string& valueName_, Ir::Intf::Type* type_) : Ir::Intf::Constant(valueName_, type_)
{
}

void Constant::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->InitFrom(emitter, type, *this);
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init constant from constant");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init constant from global");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init constant from register variable");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init constant from stack variable");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init constant from member variable");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init constant from reference variable");
}

void Constant::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init constant from parameter");
}

void Constant::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    to->AssignFrom(emitter, type, *this);
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign constant");
}

void Constant::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign constant");
}

BooleanConstant::BooleanConstant(bool value_) : Constant(value_ ? "true" : "false", Ir::Intf::GetFactory()->GetI1())
{
}

Ir::Intf::Object * CreateConstant(const std::string& name, Ir::Intf::Type* type)
{
    return new Constant(name, type);
}

Ir::Intf::Object* True()
{
    return new BooleanConstant(true);
}

Ir::Intf::Object* False()
{
    return new BooleanConstant(false);
}

Ir::Intf::Object* CreateBooleanConstant(bool value)
{
    return new BooleanConstant(value);
}

CharConstant::CharConstant(char value_) : Constant("'" + Cm::Util::CharStr(value_) + "'", Char())
{
}

Ir::Intf::Object* CreateCharConstant(char value)
{
    return new CharConstant(value);
}

StringConstant::StringConstant(const std::string& value_) : Constant(MakeStringConstantName(value_), String(int(value_.length()) + 1))
{
}

StringConstant::~StringConstant()
{
}

Ir::Intf::Object* CreateStringConstant(const std::string& value)
{
    return new StringConstant(value);
}

NullConstant::NullConstant(Ir::Intf::Type* ptrType_) : Constant("null", ptrType_)
{
}

Ir::Intf::Object* Null(Ir::Intf::Type* ptrType)
{
    return new NullConstant(ptrType);
}

I8Constant::I8Constant(int8_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI8())
{
}

Ir::Intf::Object* CreateI8Constant(int8_t value)
{
    return new I8Constant(value);
}

UI8Constant::UI8Constant(uint8_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetUI8())
{
}

Ir::Intf::Object* CreateUI8Constant(uint8_t value)
{
    return new UI8Constant(value);
}

I16Constant::I16Constant(int16_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI16())
{
}

Ir::Intf::Object* CreateI16Constant(int16_t value)
{
    return new I16Constant(value);
}

UI16Constant::UI16Constant(uint16_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetUI16())
{
}

Ir::Intf::Object* CreateUI16Constant(uint16_t value)
{
    return new UI16Constant(value);
}

I32Constant::I32Constant(int32_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI32())
{
}

Ir::Intf::Object* CreateI32Constant(int32_t value)
{
    return new I32Constant(value);
}

UI32Constant::UI32Constant(uint32_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI32())
{
}

Ir::Intf::Object* CreateUI32Constant(uint32_t value)
{
    return new UI32Constant(value);
}

I64Constant::I64Constant(int64_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI64())
{
}

Ir::Intf::Object* CreateI64Constant(int64_t value)
{
    return new I64Constant(value);
}

UI64Constant::UI64Constant(uint64_t value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetI64())
{
}

Ir::Intf::Object* CreateUI64Constant(uint64_t value)
{
    return new UI64Constant(value);
}

FloatConstant::FloatConstant(float value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetFloat())
{
}

Ir::Intf::Object* CreateFloatConstant(float value)
{
    return new FloatConstant(value);
}

DoubleConstant::DoubleConstant(double value_) : Constant(std::to_string(value_), Ir::Intf::GetFactory()->GetDouble())
{
}

Ir::Intf::Object* CreateDoubleConstant(double value)
{
    return new DoubleConstant(value);
}

} // namespace C
