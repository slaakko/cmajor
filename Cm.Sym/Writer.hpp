/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_WRITER_INCLUDED
#define CM_SYM_WRITER_INCLUDED
#include <Cm.Sym/TypeId.hpp>
#include <Cm.Ser/BinaryWriter.hpp>
#include <Cm.Util/Uuid.hpp>

namespace Cm { namespace Sym {

using Cm::Ser::BinaryWriter;
class Symbol;

class Writer
{
public:
    Writer(const std::string& fileName);
    void Write(Symbol* symbol);
    void Write(Cm::Ast::Derivation x);
    void Write(const Cm::Ast::DerivationList& derivationList);
    void Write(const TypeId& id);
    BinaryWriter& GetBinaryWriter() { return binaryWriter; }
private:
    BinaryWriter binaryWriter;
};

} } // namespace Cm::Sym

#endif // CM_SYM_WRITER_INCLUDED
