/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_READER_INCLUDED
#define CM_SYM_READER_INCLUDED
#include <Cm.Sym/TypeId.hpp>
#include <Cm.Ser/BinaryReader.hpp>
#include <Cm.Ast/TypeExpr.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Parsing/Scanner.hpp>
#include <unordered_map>
#include <forward_list>

namespace Cm { namespace Sym {

using Cm::Ser::BinaryReader;
using Cm::Parsing::Span;
class Symbol;
class SymbolTable;
class TypeSymbol;
class Value;
enum class ValueType : uint8_t;
enum class SymbolType : uint8_t;

class Reader
{
public:
    Reader(const std::string& fileName, SymbolTable& symbolTable_);
    BinaryReader& GetBinaryReader() { return binaryReader; }
    Cm::Ast::Reader& GetAstReader() { return astReader; }
    SymbolTable& GetSymbolTable() { return symbolTable; }
    Symbol* ReadSymbol();
    SymbolType ReadSymbolType();
    Span ReadSpan();
    TypeId ReadTypeId();
    Cm::Ast::Derivation ReadDerivation();
    Cm::Ast::DerivationList ReadDerivationList();
    ValueType ReadValueType();
    Value* ReadValue();
    void FetchTypeFor(Symbol* symbol, int index);
    void BackpatchType(TypeSymbol* type);
    void SetSpanFileIndexOffset(int spanFileIndexOffset_);
    void MarkSymbolsBound();
    bool AllTypesFetched();
private:
    BinaryReader binaryReader;
    SymbolTable& symbolTable;
    Cm::Ast::Reader astReader;
    typedef std::forward_list<std::pair<Symbol*, int>> FetchTypeList;
    typedef std::unordered_map<TypeId, FetchTypeList, TypeIdHash> FetchTypeMap;
    typedef FetchTypeMap::iterator FetchTypeMapIt;
    FetchTypeMap fetchTypeMap;
    int spanFileIndexOffset;
    bool markSymbolsBound;
};

} } // namespace Cm::Sym

#endif // CM_SYM_READER_INCLUDED
