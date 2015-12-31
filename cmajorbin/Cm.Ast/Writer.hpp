/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_WRITER_INCLUDED
#define CM_AST_WRITER_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <Cm.Ser/BinaryWriter.hpp>

namespace Cm { namespace Ast {

class DerivationList;
enum class Derivation : uint8_t;
enum class Specifiers : uint32_t;

using Cm::Ser::BinaryWriter;

class Writer
{
public:
    Writer(BinaryWriter& binaryWriter_);
    void Write(NodeType nodeType);
    void Write(const Span& span);
    void Write(Node* node);
    void Write(const std::string& s);
    void Write(bool b);
    void Write(char x);
    void Write(int8_t x);
    void Write(uint8_t x);
    void Write(int16_t x);
    void Write(uint16_t x);
    void Write(int32_t x);
    void Write(uint32_t x);
    void Write(int64_t x);
    void Write(uint64_t x);
    void Write(float x);
    void Write(double x);
    void Write(Derivation x);
    void Write(const DerivationList& derivationList);
    void Write(Specifiers x);
private:
    BinaryWriter& binaryWriter;
};

} } // namespace Cm::Ast

#endif // CM_AST_WRITER_INCLUDED
