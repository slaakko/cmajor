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
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Util/Uuid.hpp>

namespace Cm { namespace Sym {

using Cm::Ser::BinaryWriter;
class Symbol;
class Value;
class SymbolTable;

class Writer
{
public:
    Writer(const std::string& fileName, SymbolTable* symbolTable_);
    void Write(Symbol* symbol);
    void Write(Cm::Ast::Derivation x);
    void Write(const Cm::Ast::DerivationList& derivationList);
    void Write(const TypeId& id);
    void Write(Value* value);
    BinaryWriter& GetBinaryWriter() { return binaryWriter; }
    SymbolTable* GetSymbolTable() const { return symbolTable; }
    Cm::Ast::Writer& GetAstWriter() { return astWriter; }
private:
    BinaryWriter binaryWriter;
    SymbolTable* symbolTable;
    Cm::Ast::Writer astWriter;
};

} } // namespace Cm::Sym

#endif // CM_SYM_WRITER_INCLUDED
