/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef LLVM_IR_CONSTANT_INCLUDED
#define LLVM_IR_CONSTANT_INCLUDED
#include <Ir.Intf/Constant.hpp>

namespace Llvm { 

class Constant: public Ir::Intf::Constant
{
public:
    Constant(const std::string& valueName_, Ir::Intf::Type* type_);
    void InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar) override;
    void InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter) override;
    void AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar) override;
    void AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter) override;
};

Ir::Intf::Object* CreateConstant(const std::string& name, Ir::Intf::Type* type);

class BooleanConstant: public Constant
{
public:
    BooleanConstant(bool value_);
};

Ir::Intf::Object* True();
Ir::Intf::Object* False();

Ir::Intf::Object* CreateBooleanConstant(bool value);

class CharConstant: public Constant
{
public:
    CharConstant(char value_);
};

Ir::Intf::Object* CreateCharConstant(char value);

class StringConstant: public Constant
{
public:
    StringConstant(const std::string& value_);
	~StringConstant();
};

std::string MakeStringConstantName(const std::string& s);
std::string MakeStringConstantName(const std::string& s, bool metadataSyntax);
Ir::Intf::Object* CreateStringConstant(const std::string& value);

class NullConstant: public Constant
{
public:
    NullConstant(Ir::Intf::Type* ptrType_);
    virtual bool IsNull() const { return true; }
};

Ir::Intf::Object* Null(Ir::Intf::Type* ptrType);

class I8Constant: public Constant
{
public:
    I8Constant(int8_t value_);
};

class I16Constant: public Constant
{
public:
    I16Constant(int16_t value_);
};

class I32Constant: public Constant
{
public:
    I32Constant(int32_t value_);
};

class UI32Constant: public Constant
{
public:
    UI32Constant(uint32_t value_);
};

Ir::Intf::Object* CreateI8Constant(int8_t value);
Ir::Intf::Object* CreateI16Constant(int16_t value);
Ir::Intf::Object* CreateI32Constant(int32_t value);
Ir::Intf::Object* CreateUI32Constant(uint32_t value);

class I64Constant: public Constant
{
public:
    I64Constant(int64_t value_);
};

Ir::Intf::Object* CreateI64Constant(int64_t value);

class UI64Constant: public Constant
{
public:
    UI64Constant(uint64_t value_);
};

Ir::Intf::Object* CreateUI64Constant(uint64_t value);

class FloatConstant: public Constant
{
public:
    FloatConstant(float value_);
};

Ir::Intf::Object* CreateFloatConstant(float value);

class DoubleConstant: public Constant
{
public:
    DoubleConstant(double value_);
};

Ir::Intf::Object* CreateDoubleConstant(double value);

} // namespace Llvm

#endif // LLVM_IR_CONSTANT_INCLUDED
