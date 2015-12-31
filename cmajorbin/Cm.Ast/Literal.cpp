/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Literal.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Visitor.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <limits>

namespace Cm { namespace Ast {

BooleanLiteralNode::BooleanLiteralNode(const Span& span_) : Node(span_), value(false)
{
}

BooleanLiteralNode::BooleanLiteralNode(const Span& span_, bool value_) : Node(span_), value(value_)
{
}

Node* BooleanLiteralNode::Clone(CloneContext& cloneContext) const 
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

std::string BooleanLiteralNode::ToString() const
{
    return value ? "true" : "false";
}

void BooleanLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SByteLiteralNode::SByteLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

SByteLiteralNode::SByteLiteralNode(const Span& span_, int8_t value_) : Node(span_), value(value_)
{
}

Node* SByteLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string SByteLiteralNode::ToString() const
{
    return std::to_string(value);
}

void SByteLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ByteLiteralNode::ByteLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ByteLiteralNode::ByteLiteralNode(const Span& span_, uint8_t value_) : Node(span_), value(value_)
{
}

Node* ByteLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string ByteLiteralNode::ToString() const
{
    return std::to_string(value);
}

void ByteLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShortLiteralNode::ShortLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ShortLiteralNode::ShortLiteralNode(const Span& span_, int16_t value_) : Node(span_), value(value_)
{
}

Node* ShortLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string ShortLiteralNode::ToString() const
{
    return std::to_string(value);
}

void ShortLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UShortLiteralNode::UShortLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

UShortLiteralNode::UShortLiteralNode(const Span& span_, uint16_t value_) : Node(span_), value(value_)
{
}

Node* UShortLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string UShortLiteralNode::ToString() const
{
    return std::to_string(value);
}

void UShortLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntLiteralNode::IntLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

IntLiteralNode::IntLiteralNode(const Span& span_, int32_t value_) : Node(span_), value(value_)
{
}

Node* IntLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string IntLiteralNode::ToString() const
{
    return std::to_string(value);
}

void IntLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UIntLiteralNode::UIntLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

UIntLiteralNode::UIntLiteralNode(const Span& span_, uint32_t value_) : Node(span_), value(value_)
{
}

Node* UIntLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string UIntLiteralNode::ToString() const
{
    return std::to_string(value);
}

void UIntLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LongLiteralNode::LongLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

LongLiteralNode::LongLiteralNode(const Span& span_, int64_t value_) : Node(span_), value(value_)
{
}

Node* LongLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string LongLiteralNode::ToString() const
{
    return std::to_string(value);
}

void LongLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ULongLiteralNode::ULongLiteralNode(const Span& span_) : Node(span_), value(0)
{
}

ULongLiteralNode::ULongLiteralNode(const Span& span_, uint64_t value_) : Node(span_), value(value_)
{
}

Node* ULongLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string ULongLiteralNode::ToString() const
{
    return std::to_string(value);
}

void ULongLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

Node* FloatLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string FloatLiteralNode::ToString() const
{
    return std::to_string(value);
}

void FloatLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_) : Node(span_), value(0.0)
{
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_, double value_) : Node(span_), value(value_)
{
}

Node* DoubleLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string DoubleLiteralNode::ToString() const
{
    return std::to_string(value);
}

void DoubleLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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

Node* CharLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string CharLiteralNode::ToString() const
{
    std::string s(1, '\'');
    s.append(Cm::Util::CharStr(value)).append(1, '\'');
    return s;
}

void CharLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringLiteralNode::StringLiteralNode(const Span& span_) : Node(span_), value("")
{
}

StringLiteralNode::StringLiteralNode(const Span& span_, const std::string& value_) : Node(span_), value(value_)
{
}

Node* StringLiteralNode::Clone(CloneContext& cloneContext) const
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

std::string StringLiteralNode::ToString() const
{
    std::string s(1, '"');
    s.append(Cm::Util::StringStr(value)).append(1, '"');
    return s;
}

void StringLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NullLiteralNode::NullLiteralNode(const Span& span_) : Node(span_)
{
}

Node* NullLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new NullLiteralNode(GetSpan());
}

void NullLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace Cm::Ast
