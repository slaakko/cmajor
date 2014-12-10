/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_VALUE_INCLUDED
#define CM_BIND_VALUE_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <stdint.h>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;

enum class ValueType : uint8_t
{
    none, boolValue, charValue, sbyteValue, byteValue, shortValue, ushortValue, intValue, uintValue, longValue, ulongValue, floatValue, doubleValue, max
};

std::string ValueTypeStr(ValueType valueType);
ValueType GetValueTypeFor(SymbolType symbolType);
ValueType GetCommonType(ValueType left, ValueType right);

class Value
{
public:
    virtual ~Value();
    virtual Value* Clone() const = 0;
    virtual ValueType GetValueType() const = 0;
    virtual Value* As(ValueType targetType, bool cast, const Span& span) const = 0;
};

class BoolValue : public Value
{
public:
    typedef bool OperandType;
    BoolValue(bool value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::boolValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    bool Value() const { return value; }
private:
    bool value;
};

class CharValue : public Value
{
public:
    typedef char OperandType;
    CharValue(char value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::charValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    char Value() const { return value; }
private:
    char value;
};

class SByteValue : public Value
{
public:
    typedef int8_t OperandType;
    SByteValue(int8_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::sbyteValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    int8_t Value() const { return value; }
private:
    int8_t value;
};

class ByteValue : public Value
{
public:
    typedef uint8_t OperandType;
    ByteValue(uint8_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::byteValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    uint8_t Value() const { return value; }
private:
    uint8_t value;
};

class ShortValue : public Value
{
public:
    typedef int16_t OperandType;
    ShortValue(int16_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::shortValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    int16_t Value() const { return value; }
private:
    int16_t value;
};

class UShortValue : public Value
{
public:
    typedef uint16_t OperandType;
    UShortValue(uint16_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::ushortValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    uint16_t Value() const { return value; }
private:
    uint16_t value;
};

class IntValue : public Value
{
public:
    typedef int32_t OperandType;
    IntValue(int32_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::intValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    int32_t Value() const { return value; }
private:
    int32_t value;
};

class UIntValue : public Value
{
public:
    typedef uint32_t OperandType;
    UIntValue(uint32_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::uintValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    uint32_t Value() const { return value; }
private:
    uint32_t value;
};

class LongValue : public Value
{
public:
    typedef int64_t OperandType;
    LongValue(int64_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::longValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    int64_t Value() const { return value; }
private:
    int64_t value;
};

class ULongValue : public Value
{
public:
    typedef uint64_t OperandType;
    ULongValue(uint64_t value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::ulongValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    uint64_t Value() const { return value; }
private:
    uint64_t value;
};

class FloatValue : public Value
{
public:
    typedef float OperandType;
    FloatValue(float value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::floatValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    float Value() const { return value; }
private:
    float value;
};

class DoubleValue : public Value
{
public:
    typedef double OperandType;
    DoubleValue(double value_);
    Value* Clone() const override;
    ValueType GetValueType() const override { return ValueType::doubleValue; }
    Value* As(ValueType targetType, bool cast, const Span& span) const override;
    double Value() const { return value; }
private:
    double value;
};

} } // namespace Cm::Sym

#endif // CM_BIND_VALUE_INCLUDED

