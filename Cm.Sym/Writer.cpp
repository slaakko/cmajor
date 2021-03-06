/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/BasicTypeSymbol.hpp>
#include <Cm.Sym/Value.hpp>

namespace Cm {  namespace Sym {

Writer::Writer(const std::string& fileName, SymbolTable* symbolTable_): binaryWriter(fileName), symbolTable(symbolTable_), astWriter(binaryWriter), exportMemberVariablesAndFunctionSymbols(false)
{
}

void Writer::Write(Symbol* symbol)
{
    if (symbol->IsBasicTypeSymbol())
    {
        binaryWriter.Write(uint8_t(symbol->GetSymbolType()));
    }
    else
    {
        binaryWriter.Write(uint8_t(symbol->GetSymbolType()));
        const Span& span = symbol->GetSpan();
        binaryWriter.Write(&span, sizeof(Span));
        binaryWriter.Write(symbol->Name());
        symbol->Write(*this);
    }
}

void Writer::Write(Cm::Ast::Derivation x)
{
    uint8_t d = *reinterpret_cast<uint8_t*>(&x);
    binaryWriter.Write(d);
}

void Writer::Write(const Cm::Ast::DerivationList& derivationList)
{
    uint8_t n = derivationList.NumDerivations();
    binaryWriter.Write(n);
    for (uint8_t i = 0; i < n; ++i)
    {
        Write(derivationList[i]);
    }
}

void Writer::Write(const TypeId& id)
{
    binaryWriter.Write(id.Rep(), typeIdRepSize);
}

void Writer::Write(Value* value)
{
    Cm::Sym::ValueType valueType = value->GetValueType();
    binaryWriter.Write(uint8_t(valueType));
    value->Write(*this);
}

void Writer::PushExportMemberVariablesAndFunctionSymbols(bool export_)
{
    exportMemberVariablesAndFunctionSymbolsStack.push(exportMemberVariablesAndFunctionSymbols);
    exportMemberVariablesAndFunctionSymbols = export_;
}

void Writer::PopExportMemberVariablesAndFunctionSymbols()
{
    exportMemberVariablesAndFunctionSymbols = exportMemberVariablesAndFunctionSymbolsStack.top();
    exportMemberVariablesAndFunctionSymbolsStack.pop();
}

} } // namespace Cm::Sym
