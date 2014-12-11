/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/TypeExpr.hpp>

namespace Cm { namespace Ast {

Writer::Writer(const std::string& fileName) : binaryWriter(fileName)
{
}

void Writer::Write(NodeType nodeType)
{
    uint8_t x = *reinterpret_cast<uint8_t*>(&nodeType);
    binaryWriter.Write(x);
}

void Writer::Write(const Span& span)
{
    binaryWriter.Write(&span, sizeof(Span));
}

void Writer::Write(Node* node)
{
    Write(node->GetNodeType());
    Write(node->GetSpan());
    node->Write(*this);
}

void Writer::Write(const std::string& s)
{
    binaryWriter.Write(s);
}

void Writer::Write(bool b)
{
    binaryWriter.Write(b);
}

void Writer::Write(char x)
{
    binaryWriter.Write(x);
}

void Writer::Write(int8_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(uint8_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(int16_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(uint16_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(int32_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(uint32_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(int64_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(uint64_t x)
{
    binaryWriter.Write(x);
}

void Writer::Write(float x)
{
    binaryWriter.Write(x);
}

void Writer::Write(double x)
{
    binaryWriter.Write(x);
}

void Writer::Write(Derivation x)
{
    uint8_t d = *reinterpret_cast<uint8_t*>(&x);
    binaryWriter.Write(d);
}

void Writer::Write(const DerivationList& derivationList)
{
    uint8_t n = derivationList.NumDerivations();
    binaryWriter.Write(n);
    for (uint8_t i = 0; i < n; ++i)
    {
        Write(derivationList[i]);
    }
}

void Writer::Write(Specifiers x)
{
    uint16_t s = *reinterpret_cast<uint16_t*>(&x);
    binaryWriter.Write(s);
}

} } // namespace Cm::Ast
