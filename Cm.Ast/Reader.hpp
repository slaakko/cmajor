/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_READER_INCLUDED
#define CM_AST_READER_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ser/BinaryReader.hpp>

namespace Cm { namespace Ast {

using Cm::Ser::BinaryReader;
enum class Derivation : uint8_t;
class DerivationList;
class IdentifierNode;
enum class Specifiers : uint16_t;

class Reader
{
public:
    Reader(const std::string& fileName);
    NodeType ReadNodeType();
    Span ReadSpan();
    Node* ReadNode();
    IdentifierNode* ReadIdentifierNode();
    std::string ReadString();
    bool ReadBool();
    int8_t ReadSByte();
    uint8_t ReadByte();
    int16_t ReadShort();
    int16_t ReadUShort();
    int32_t ReadInt();
    uint32_t ReadUInt();
    int64_t ReadLong();
    uint64_t ReadULong();
    float ReadFloat();
    double ReadDouble();
    char ReadChar();
    Derivation ReadDerivation();
    DerivationList ReadDerivationList();
    Specifiers ReadSpecifiers();
private:
    BinaryReader binaryReader;
};

} } // namespace Cm::Ast

#endif // CM_AST_READER_INCLUDED

