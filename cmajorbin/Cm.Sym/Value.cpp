/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/Exception.hpp>

namespace Cm { namespace Sym {

ValueType valueTypes[uint8_t(Cm::Sym::SymbolType::maxSymbol)] =
{
    ValueType::boolValue, ValueType::sbyteValue, ValueType::byteValue, ValueType::shortValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue,
    ValueType::floatValue, ValueType::doubleValue, ValueType::charValue, ValueType::none,
    ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
    ValueType::none, ValueType::none, ValueType::none
};

ValueType GetValueTypeFor(SymbolType symbolType)
{
    ValueType valueType = valueTypes[uint8_t(symbolType)];
    if (valueType == ValueType::none)
    {
        throw std::runtime_error("invalid value type for symbol type");
    }
    return valueType;
}

ValueType commonType[uint8_t(Cm::Sym::ValueType::max)][uint8_t(Cm::Sym::ValueType::max)] = 
{
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::boolValue, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::charValue, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::sbyteValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue},
    { ValueType::none, ValueType::none, ValueType::none, ValueType::shortValue, ValueType::byteValue, ValueType::shortValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::shortValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue},
    { ValueType::none, ValueType::none, ValueType::none, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::doubleValue },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue }
};

ValueType GetCommonType(ValueType left, ValueType right)
{
    return commonType[uint8_t(left)][uint8_t(right)];
}

const char* valueTypeStr[uint8_t(ValueType::max)] =
{
    "", "bool", "char", "sbyte", "byte", "short", "ushort", "int", "uint", "long", "ulong", "float", "double"
};

std::string ValueTypeStr(ValueType valueType)
{
    return valueTypeStr[uint8_t(valueType)];
}

Value::~Value()
{
}

BoolValue::BoolValue(bool value_) : value(value_)
{
}

Value* BoolValue::Clone() const
{
    return new BoolValue(value);
}

Value* BoolValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            return new BoolValue(value);
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(static_cast<float>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(static_cast<double>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

CharValue::CharValue(char value_) : value(value_)
{
}

Value* CharValue::Clone() const
{
    return new CharValue(value);
}

Value* CharValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            return new CharValue(value);
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(static_cast<float>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::doubleValue:
        {
            if (cast)
            {
                return new DoubleValue(static_cast<double>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

SByteValue::SByteValue(int8_t value_) : value(value_)
{
}

Value* SByteValue::Clone() const
{
    return new SByteValue(value);
}

Value* SByteValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            return new SByteValue(value);
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            return new ShortValue(value);
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

ByteValue::ByteValue(uint8_t value_) : value(value_)
{
}

Value* ByteValue::Clone() const
{
    return new ByteValue(value);
}

Value* ByteValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            return new ByteValue(value);
        }
        case ValueType::shortValue:
        {
            return new ShortValue(value);
        }
        case ValueType::ushortValue:
        {
            return new UShortValue(value);
        }
        case ValueType::intValue:
        {
            return new IntValue(value);
        }
        case ValueType::uintValue:
        {
            return new UIntValue(value);
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

ShortValue::ShortValue(int16_t value_) : value(value_)
{
}

Value* ShortValue::Clone() const
{
    return new ShortValue(value);
}

Value* ShortValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            return new ShortValue(value);
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

UShortValue::UShortValue(uint16_t value_) : value(value_)
{
}

Value* UShortValue::Clone() const
{
    return new UShortValue(value);
}

Value* UShortValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            return new UShortValue(value);
        }
        case ValueType::intValue:
        {
            return new IntValue(value);
        }
        case ValueType::uintValue:
        {
            return new UIntValue(value);
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

IntValue::IntValue(int32_t value_) : value(value_)
{
}

Value* IntValue::Clone() const
{
    return new IntValue(value);
}

Value* IntValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            return new IntValue(value);
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<uint32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        { 
            throw Exception("invalid conversion", span);
        }
    }
}

UIntValue::UIntValue(uint32_t value_) : value(value_)
{
}

Value* UIntValue::Clone() const
{
    return new UIntValue(value);
}

Value* UIntValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            return new UIntValue(value);
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

LongValue::LongValue(int64_t value_) : value(value_)
{
}

Value* LongValue::Clone() const
{
    return new LongValue(value);
}

Value* LongValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            return new LongValue(value);
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

ULongValue::ULongValue(uint64_t value_) : value(value_)
{
}

Value* ULongValue::Clone() const
{
    return new ULongValue(value);
}

Value* ULongValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ulongValue:
        {
            return new ULongValue(value);
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

FloatValue::FloatValue(float value_) : value(value_)
{
}

Value* FloatValue::Clone() const
{
    return new FloatValue(value);
}

Value* FloatValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType)
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            return new FloatValue(value);
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

DoubleValue::DoubleValue(double value_) : value(value_)
{
}

Value* DoubleValue::Clone() const
{
    return new DoubleValue(value);
}

Value* DoubleValue::As(ValueType targetType, bool cast, const Span& span) const
{
    switch (targetType) 
    {
        case ValueType::boolValue:
        {
            if (cast)
            {
                return new BoolValue(static_cast<bool>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::charValue:
        {
            if (cast)
            {
                return new CharValue(static_cast<char>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::sbyteValue:
        {
            if (cast)
            {
                return new SByteValue(static_cast<int8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::byteValue:
        {
            if (cast)
            {
                return new ByteValue(static_cast<uint8_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::shortValue:
        {
            if (cast)
            {
                return new ShortValue(static_cast<int16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ushortValue:
        {
            if (cast)
            {
                return new UShortValue(static_cast<uint16_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::intValue:
        {
            if (cast)
            {
                return new IntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::uintValue:
        {
            if (cast)
            {
                return new UIntValue(static_cast<int32_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::longValue:
        {
            if (cast)
            {
                return new LongValue(static_cast<int64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::ulongValue:
        {
            if (cast)
            {
                return new ULongValue(static_cast<uint64_t>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::floatValue:
        {
            if (cast)
            {
                return new FloatValue(static_cast<float>(value));
            }
            else
            {
                throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType) + " without a cast", span);
            }
        }
        case ValueType::doubleValue:
        {
            return new DoubleValue(value);
        }
        default:
        {
            throw Exception("invalid conversion", span);
        }
    }
}

} } // namespace Cm::Sym
