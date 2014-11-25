/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <limits>

namespace Cm { namespace Ast {

BooleanLiteralNode::BooleanLiteralNode(const Span& span_) : Node(span_), value(false)
{
}

BooleanLiteralNode::BooleanLiteralNode(const Span& span_, bool value_) : Node(span_), value(value_)
{
}

Node* BooleanLiteralNode::Clone() const 
{
    return new BooleanLiteralNode(GetSpan(), value);
}

void BooleanLiteralNode::Read(Reader& reader)
{
    value = reader.ReadBool();
}

void BooleanLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

SByteLiteralNode::SByteLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

SByteLiteralNode::SByteLiteralNode(const Span& span_, int8_t value_) : Node(span_), value(value_)
{
}

Node* SByteLiteralNode::Clone() const
{
    return new SByteLiteralNode(GetSpan(), value);
}

void SByteLiteralNode::Read(Reader& reader)
{
    value = reader.ReadSByte();
}

void SByteLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

ByteLiteralNode::ByteLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ByteLiteralNode::ByteLiteralNode(const Span& span_, uint8_t value_) : Node(span_), value(value_)
{
}

Node* ByteLiteralNode::Clone() const
{
    return new ByteLiteralNode(GetSpan(), value);
}

void ByteLiteralNode::Read(Reader& reader)
{
    value = reader.ReadByte();
}

void ByteLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

ShortLiteralNode::ShortLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ShortLiteralNode::ShortLiteralNode(const Span& span_, int16_t value_) : Node(span_), value(value_)
{
}

Node* ShortLiteralNode::Clone() const
{
    return new ShortLiteralNode(GetSpan(), value);
}

void ShortLiteralNode::Read(Reader& reader)
{
    value = reader.ReadShort();
}

void ShortLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

UShortLiteralNode::UShortLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

UShortLiteralNode::UShortLiteralNode(const Span& span_, uint16_t value_) : Node(span_), value(value_)
{
}

Node* UShortLiteralNode::Clone() const
{
    return new UShortLiteralNode(GetSpan(), value);
}

void UShortLiteralNode::Read(Reader& reader)
{
    value = reader.ReadUShort();
}

void UShortLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

IntLiteralNode::IntLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

IntLiteralNode::IntLiteralNode(const Span& span_, int32_t value_) : Node(span_), value(value_)
{
}

Node* IntLiteralNode::Clone() const
{
    return new IntLiteralNode(GetSpan(), value);
}

void IntLiteralNode::Read(Reader& reader)
{
    value = reader.ReadInt();
}

void IntLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

UIntLiteralNode::UIntLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

UIntLiteralNode::UIntLiteralNode(const Span& span_, uint32_t value_) : Node(span_), value(value_)
{
}

Node* UIntLiteralNode::Clone() const
{
    return new UIntLiteralNode(GetSpan(), value);
}

void UIntLiteralNode::Read(Reader& reader)
{
    value = reader.ReadUInt();
}

void UIntLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

LongLiteralNode::LongLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

LongLiteralNode::LongLiteralNode(const Span& span_, int64_t value_) : Node(span_), value(value_)
{
}

Node* LongLiteralNode::Clone() const
{
    return new LongLiteralNode(GetSpan(), value);
}

void LongLiteralNode::Read(Reader& reader)
{
    value = reader.ReadLong();
}

void LongLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

ULongLiteralNode::ULongLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ULongLiteralNode::ULongLiteralNode(const Span& span_, uint64_t value_) : Node(span_), value(value_)
{
}

Node* ULongLiteralNode::Clone() const
{
    return new ULongLiteralNode(GetSpan(), value);
}

void ULongLiteralNode::Read(Reader& reader)
{
    value = reader.ReadULong();
}

void ULongLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

Node* CreateIntegerLiteralNode(const Span& span, uint64_t value, bool unsignedSuffix)
{
    if (unsignedSuffix)
    {
        if (value <= std::numeric_limits<uint8_t>::max()) return new ByteLiteralNode(span, static_cast<uint8_t>(value));
        if (value <= std::numeric_limits<uint16_t>::max()) return new UShortLiteralNode(span, static_cast<uint16_t>(value));
        if (value <= std::numeric_limits<uint32_t>::max()) return new UIntLiteralNode(span, static_cast<uint32_t>(value));
        return new ULongLiteralNode(span, value);
    }
    else
    {
        if (value <= std::numeric_limits<int8_t>::max()) return new SByteLiteralNode(span, static_cast<int8_t>(value));
        if (value <= std::numeric_limits<uint8_t>::max()) return new ByteLiteralNode(span, static_cast<uint8_t>(value));
        if (value <= std::numeric_limits<int16_t>::max()) return new ShortLiteralNode(span, static_cast<int16_t>(value));
        if (value <= std::numeric_limits<uint16_t>::max()) return new UShortLiteralNode(span, static_cast<uint16_t>(value));
        if (value <= std::numeric_limits<int32_t>::max()) return new IntLiteralNode(span, static_cast<int32_t>(value));
        if (value <= std::numeric_limits<uint32_t>::max()) return new UIntLiteralNode(span, static_cast<uint32_t>(value));
        if (value <= std::numeric_limits<int64_t>::max()) return new LongLiteralNode(span, static_cast<int64_t>(value));
        return new ULongLiteralNode(span, value);
    }
}

FloatLiteralNode::FloatLiteralNode(const Span& span_) : Node(span_), value(0.0f)
{
}

FloatLiteralNode::FloatLiteralNode(const Span& span_, float value_) : Node(span_), value(value_)
{
}

Node* FloatLiteralNode::Clone() const
{
    return new FloatLiteralNode(GetSpan(), value);
}

void FloatLiteralNode::Read(Reader& reader)
{
    value = reader.ReadFloat();
}

void FloatLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_) : Node(span_), value(0.0)
{
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_, double value_) : Node(span_), value(value_)
{
}

Node* DoubleLiteralNode::Clone() const
{
    return new DoubleLiteralNode(GetSpan(), value);
}

void DoubleLiteralNode::Read(Reader& reader)
{
    value = reader.ReadDouble();
}

void DoubleLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

Node* CreateFloatingLiteralNode(const Span& span, double value, bool float_)
{
    if (float_)
    {
        return new FloatLiteralNode(span, static_cast<float>(value));
    }
    else
    {
        return new DoubleLiteralNode(span, value);
    }
}

CharLiteralNode::CharLiteralNode(const Span& span_) : Node(span_), value('\0')
{
}

CharLiteralNode::CharLiteralNode(const Span& span_, char value_) : Node(span_), value(value_)
{
}

Node* CharLiteralNode::Clone() const
{
    return new CharLiteralNode(GetSpan(), value);
}

void CharLiteralNode::Read(Reader& reader)
{
    value = reader.ReadChar();
}

void CharLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

StringLiteralNode::StringLiteralNode(const Span& span_) : Node(span_), value("")
{
}

StringLiteralNode::StringLiteralNode(const Span& span_, const std::string& value_) : Node(span_), value(value_)
{
}

Node* StringLiteralNode::Clone() const
{
    return new StringLiteralNode(GetSpan(), value);
}

void StringLiteralNode::Read(Reader& reader)
{
    value = reader.ReadString();
}

void StringLiteralNode::Write(Writer& writer)
{
    writer.Write(value);
}

NullLiteralNode::NullLiteralNode(const Span& span_) : Node(span_)
{
}

Node* NullLiteralNode::Clone() const
{
    return new NullLiteralNode(GetSpan());
}

} } // namespace Cm::Ast
