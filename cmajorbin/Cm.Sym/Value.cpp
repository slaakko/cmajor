/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Value.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.IrIntf/Rep.hpp>
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Sym {

ValueType valueTypes[uint8_t(Cm::Sym::SymbolType::maxSymbol)] =
{
    ValueType::boolValue, ValueType::charValue, ValueType::none,
    ValueType::sbyteValue, ValueType::byteValue, ValueType::shortValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue,
    ValueType::floatValue, ValueType::doubleValue, ValueType::nullValue, ValueType::none,
    ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none,
    ValueType::none, ValueType::none, ValueType::none
};

ValueType GetValueTypeFor(SymbolType symbolType)
{
    ValueType valueType = valueTypes[uint8_t(symbolType)];
    if (valueType == ValueType::none)
    {
        throw std::runtime_error("invalid basic value type for symbol type");
    }
    return valueType;
}

ValueType commonType[uint8_t(Cm::Sym::ValueType::max)][uint8_t(Cm::Sym::ValueType::max)] = 
{
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::boolValue, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::charValue, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::sbyteValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::shortValue, ValueType::byteValue, ValueType::shortValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::shortValue, ValueType::shortValue, ValueType::shortValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::ushortValue, ValueType::intValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::intValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::uintValue, ValueType::longValue, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::longValue, ValueType::none, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::none, ValueType::ulongValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::floatValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::doubleValue, ValueType::none, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::nullValue, ValueType::none },
    { ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::none, ValueType::stringValue }
};

ValueType GetCommonType(ValueType left, ValueType right)
{
    return commonType[uint8_t(left)][uint8_t(right)];
}

const char* valueTypeStr[uint8_t(ValueType::max)] =
{
    "", "bool", "char", "sbyte", "byte", "short", "ushort", "int", "uint", "long", "ulong", "float", "double", "@nullptrtype", "string"
};

std::string ValueTypeStr(ValueType valueType)
{
    return valueTypeStr[uint8_t(valueType)];
}

Value::~Value()
{
}

BoolValue::BoolValue() : value(false)
{
}

BoolValue::BoolValue(bool value_) : value(value_)
{
}

Value* BoolValue::Clone() const
{
    return new BoolValue(value);
}

void BoolValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadBool();
}

void BoolValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* BoolValue::CreateIrObject() const
{
    return value ? Cm::IrIntf::True() : Cm::IrIntf::False();
}

CharValue::CharValue() : value('\0')
{
}

CharValue::CharValue(char value_) : value(value_)
{
}

Value* CharValue::Clone() const
{
    return new CharValue(value);
}

void CharValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadChar();
}

void CharValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* CharValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateCharConstant(value);
}

SByteValue::SByteValue() : value(0)
{
}

SByteValue::SByteValue(int8_t value_) : value(value_)
{
}

Value* SByteValue::Clone() const
{
    return new SByteValue(value);
}

void SByteValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadSByte();
}

void SByteValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* SByteValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateI8Constant(value);
}

ByteValue::ByteValue() : value(0)
{
}

ByteValue::ByteValue(uint8_t value_) : value(value_)
{
}

Value* ByteValue::Clone() const
{
    return new ByteValue(value);
}

void ByteValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadByte();
}

void ByteValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* ByteValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateUI8Constant(value);
}

ShortValue::ShortValue() : value(0)
{
}

ShortValue::ShortValue(int16_t value_) : value(value_)
{
}

Value* ShortValue::Clone() const
{
    return new ShortValue(value);
}

void ShortValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadShort();
}

void ShortValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* ShortValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateI16Constant(value);
}

UShortValue::UShortValue() : value(0)
{
}

UShortValue::UShortValue(uint16_t value_) : value(value_)
{
}

Value* UShortValue::Clone() const
{
    return new UShortValue(value);
}

void UShortValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadUShort();
}

void UShortValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* UShortValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateUI16Constant(value);
}

IntValue::IntValue() : value(0)
{
}

IntValue::IntValue(int32_t value_) : value(value_)
{
}

Value* IntValue::Clone() const
{
    return new IntValue(value);
}

void IntValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadInt();
}

void IntValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* IntValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateI32Constant(value);
}

UIntValue::UIntValue() : value(0)
{
}

UIntValue::UIntValue(uint32_t value_) : value(value_)
{
}

Value* UIntValue::Clone() const
{
    return new UIntValue(value);
}

void UIntValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadUInt();
}

void UIntValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* UIntValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateUI32Constant(value);
}

LongValue::LongValue() : value(0)
{
}

LongValue::LongValue(int64_t value_) : value(value_)
{
}

Value* LongValue::Clone() const
{
    return new LongValue(value);
}

void LongValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadLong();
}

void LongValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* LongValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateI64Constant(value);
}

ULongValue::ULongValue() : value(0)
{
}

ULongValue::ULongValue(uint64_t value_) : value(value_)
{
}

Value* ULongValue::Clone() const
{
    return new ULongValue(value);
}

void ULongValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadULong();
}

void ULongValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* ULongValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateUI64Constant(value);
}

FloatValue::FloatValue() : value(0.0f)
{
}

FloatValue::FloatValue(float value_) : value(value_)
{
}

Value* FloatValue::Clone() const
{
    return new FloatValue(value);
}

void FloatValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadFloat();
}

void FloatValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* FloatValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateFloatConstant(value);
}

DoubleValue::DoubleValue() : value(0.0)
{
}

DoubleValue::DoubleValue(double value_) : value(value_)
{
}

Value* DoubleValue::Clone() const
{
    return new DoubleValue(value);
}

void DoubleValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadDouble();
}

void DoubleValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
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

Ir::Intf::Object* DoubleValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateDoubleConstant(value);
}

NullValue::NullValue() : type(nullptr)
{
}

Value* NullValue::Clone() const
{
    return new NullValue();
}

void NullValue::Read(Reader& reader)
{
    bool hasType = reader.GetBinaryReader().ReadBool();
    if (hasType)
    {
        uint32_t sid = reader.GetBinaryReader().ReadUInt();
        Symbol* symbol = reader.GetSymbolTable().GetSymbol(sid);
        if (!symbol)
        {
            throw std::runtime_error("could not get type symbol for sid " + std::to_string(sid) + " from symbol table");
        }
        if (symbol->IsTypeSymbol())
        {
            type = static_cast<TypeSymbol*>(symbol);
        }
        else
        {
            throw std::runtime_error("type symbol expected");
        }
    }
}

void NullValue::Write(Writer& writer)
{
    bool hasType = type != nullptr;
    writer.GetBinaryWriter().Write(hasType);
    if (hasType)
    {
        uint32_t sid = type->Sid();
        type->DoSerialize();
        writer.GetBinaryWriter().Write(sid);
    }
}

Value* NullValue::As(ValueType targetType, bool cast, const Span& span) const
{
    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType), span);
}

Ir::Intf::Object* NullValue::CreateIrObject() const
{
    Ir::Intf::Type* irType = nullptr;
    if (type)
    {
        irType = type->GetIrType();
    }
    else
    {
        irType = Cm::IrIntf::Pointer(Ir::Intf::GetFactory()->GetI1(), 1);
    }
    return Cm::IrIntf::Null(irType);
}

StringValue::StringValue() : value("")
{
}

StringValue::StringValue(const std::string& value_) : value(value_)
{
}


Value* StringValue::Clone() const 
{
    return new StringValue(value);
}

void StringValue::Read(Reader& reader)
{
    value = reader.GetBinaryReader().ReadString();
}

void StringValue::Write(Writer& writer)
{
    writer.GetBinaryWriter().Write(value);
}

Value* StringValue::As(ValueType targetType, bool cast, const Span& span) const
{
    throw Exception("cannot convert " + ValueTypeStr(GetValueType()) + " to " + ValueTypeStr(targetType), span);
}

Ir::Intf::Object* StringValue::CreateIrObject() const
{
    return Cm::IrIntf::CreateStringConstant(value);
}

} } // namespace Cm::Sym
