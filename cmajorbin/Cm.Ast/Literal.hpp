/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_LITERAL_INCLUDED
#define CM_AST_LITERAL_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class BooleanLiteralNode : public Node
{
public:
    BooleanLiteralNode(const Span& span_);
    BooleanLiteralNode(const Span& span_, bool value_);
    NodeType GetType() const override { return NodeType::booleanLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    bool value;
};

class SByteLiteralNode : public Node
{
public:
    SByteLiteralNode(const Span& span_);
    SByteLiteralNode(const Span& span_, int8_t value_);
    NodeType GetType() const override { return NodeType::sbyteLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    int8_t value;
};

class ByteLiteralNode : public Node
{
public:
    ByteLiteralNode(const Span& span_);
    ByteLiteralNode(const Span& span_, uint8_t value_);
    NodeType GetType() const override { return NodeType::byteLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    uint8_t value;
};

class ShortLiteralNode : public Node
{
public:
    ShortLiteralNode(const Span& span_);
    ShortLiteralNode(const Span& span_, int16_t value_);
    NodeType GetType() const override { return NodeType::shortLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    int16_t value;
};

class UShortLiteralNode : public Node
{
public:
    UShortLiteralNode(const Span& span_);
    UShortLiteralNode(const Span& span_, uint16_t value_);
    NodeType GetType() const override { return NodeType::ushortLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    uint16_t value;
};

class IntLiteralNode : public Node
{
public:
    IntLiteralNode(const Span& span_);
    IntLiteralNode(const Span& span_, int32_t value_);
    NodeType GetType() const override { return NodeType::intLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    int32_t value;
};

class UIntLiteralNode : public Node
{
public:
    UIntLiteralNode(const Span& span_);
    UIntLiteralNode(const Span& span_, uint32_t value_);
    NodeType GetType() const override { return NodeType::uintLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    uint32_t value;
};

class LongLiteralNode : public Node
{
public:
    LongLiteralNode(const Span& span_);
    LongLiteralNode(const Span& span_, int64_t value_);
    NodeType GetType() const override { return NodeType::longLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    int64_t value;
};

class ULongLiteralNode : public Node
{
public:
    ULongLiteralNode(const Span& span_);
    ULongLiteralNode(const Span& span_, uint64_t value_);
    NodeType GetType() const override { return NodeType::ulongLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    uint64_t value;
};

Node* CreateIntegerLiteralNode(const Span& span, uint64_t value, bool unsignedSuffix);

class FloatLiteralNode : public Node
{
public:
    FloatLiteralNode(const Span& span_);
    FloatLiteralNode(const Span& span_, float value_);
    NodeType GetType() const override { return NodeType::floatLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    float value;
};

class DoubleLiteralNode : public Node
{
public:
    DoubleLiteralNode(const Span& span_);
    DoubleLiteralNode(const Span& span_, double value_);
    NodeType GetType() const override { return NodeType::doubleLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    double value;
};

Node* CreateFloatingLiteralNode(const Span& span, double value, bool float_);

class CharLiteralNode : public Node
{
public:
    CharLiteralNode(const Span& span_);
    CharLiteralNode(const Span& span_, char value_);
    NodeType GetType() const override { return NodeType::charLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    char value;
};

class StringLiteralNode : public Node
{
public:
    StringLiteralNode(const Span& span_);
    StringLiteralNode(const Span& span_, const std::string& value_);
    NodeType GetType() const override { return NodeType::stringLiteralNode; }
    void Read(Reader& reader) override;
    void Write(Writer& writer) override;
private:
    std::string value;
};

class NullLiteralNode : public Node
{
public:
    NullLiteralNode(const Span& span_);
    NodeType GetType() const override { return NodeType::nullLiteralNode; }
};

} } // namespace Cm::Ast

#endif // CM_AST_LITERAL_INCLUDED
