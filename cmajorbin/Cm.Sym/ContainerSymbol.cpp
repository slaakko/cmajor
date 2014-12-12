/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

ContainerSymbol::ContainerSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), containerScope()
{
    containerScope.SetContainer(this);
}

void ContainerSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    std::vector<Symbol*> exportSymbols;
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        if (symbol->IsExportSymbol())
        {
            exportSymbols.push_back(symbol.get());
        }
    }
    int32_t n = int32_t(exportSymbols.size());
    writer.GetBinaryWriter().Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        writer.Write(exportSymbols[i]);
    }
}

void ContainerSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    int32_t n = reader.GetBinaryReader().ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol();
        AddSymbol(symbol);
    }
}

void ContainerSymbol::AddSymbol(Symbol* symbol)
{
    if (!symbol->Name().empty())
    {
        containerScope.Install(symbol);
    }
    symbols.push_back(std::unique_ptr<Symbol>(symbol));
    symbol->SetParent(this);
}

void ContainerSymbol::AddFunctionSymbol(FunctionSymbol* functionSymbol)
{
    symbols.push_back(std::unique_ptr<Symbol>(functionSymbol));
    functionSymbol->SetParent(this);
}

void ContainerSymbol::AddTemplateTypeSymbol(TemplateTypeSymbol* templateTypeSymbol)
{
    symbols.push_back(std::unique_ptr<Symbol>(templateTypeSymbol));
    templateTypeSymbol->SetParent(this);
}

void ContainerSymbol::Dump(CodeFormatter& formatter)
{
    formatter.Write("begin "); 
    Symbol::Dump(formatter);
    formatter.IncIndent();
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        symbol->Dump(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("end " + Name());
}

} } // namespace Cm::Sym
