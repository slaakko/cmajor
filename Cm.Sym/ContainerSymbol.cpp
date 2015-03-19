/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.Sym/FunctionGroupSymbol.hpp>
#include <Cm.Sym/ConceptGroupSymbol.hpp>
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <Cm.Sym/NamespaceSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/Exception.hpp>
#include <Cm.Sym/ClassTypeSymbol.hpp>

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
    if (!symbol->Name().empty() && !symbol->IsFunctionSymbol() && !symbol->IsConceptSymbol())
    {
        containerScope.Install(symbol);
    }
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        if (!functionSymbol->IsFunctionTemplateSpecialization()) // function template specializations are not added to function group
        {
            FunctionGroupSymbol* functionGroupSymbol = MakeFunctionGroupSymbol(functionSymbol->GroupName(), functionSymbol->GetSpan());
            functionGroupSymbol->AddFunction(functionSymbol);
        }
    }
    else if (symbol->IsConceptSymbol())
    {
        ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(symbol);
        ConceptGroupSymbol* conceptGroupSymbol = MakeConceptGroupSymbol(conceptSymbol->GroupName(), conceptSymbol->GetSpan());
        conceptGroupSymbol->AddConcept(conceptSymbol);
    }
    if (!symbol->IsTemplateTypeSymbol())
    {
        symbols.push_back(std::unique_ptr<Symbol>(symbol));
    }
    symbol->SetParent(this);
}

FunctionGroupSymbol* ContainerSymbol::MakeFunctionGroupSymbol(const std::string& groupName, const Span& span)
{
    Cm::Sym::Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        FunctionGroupSymbol* functionGroupSymbol = new FunctionGroupSymbol(span, groupName, &containerScope);
        functionGroupSymbol->SetPublic();
        AddSymbol(functionGroupSymbol);
        return functionGroupSymbol;
    }
    if (symbol->IsFunctionGroupSymbol())
    {
        FunctionGroupSymbol* functionGroupSymbol = static_cast<FunctionGroupSymbol*>(symbol);
        return functionGroupSymbol;
    }
    else
    {
        throw Exception("name of symbol '" + symbol->FullName() + "' conflicts with a function group '" + groupName + "'", symbol->GetSpan(), span);
    }
}

ConceptGroupSymbol* ContainerSymbol::MakeConceptGroupSymbol(const std::string& groupName, const Span& span)
{
    Cm::Sym::Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        ConceptGroupSymbol* conceptGroupSymbol = new ConceptGroupSymbol(span, groupName, &containerScope);
        conceptGroupSymbol->SetPublic();
        AddSymbol(conceptGroupSymbol);
        return conceptGroupSymbol;
    }
    if (symbol->IsConceptGroupSymbol())
    {
        ConceptGroupSymbol* conceptGroupSymbol = static_cast<ConceptGroupSymbol*>(symbol);
        return conceptGroupSymbol;
    }
    else
    {
        throw Exception("name of symbol '" + symbol->FullName() + "' conflicts with a concept group '" + groupName + "'", symbol->GetSpan(), span);
    }
}

void ContainerSymbol::Dump(CodeFormatter& formatter)
{
    Symbol::Dump(formatter);
    formatter.IncIndent();
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        symbol->Dump(formatter);
    }
    formatter.DecIndent();
}

void ContainerSymbol::CollectExportedDerivedTypes(std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        if (symbol->IsExportSymbol())
        {
            symbol->CollectExportedDerivedTypes(exportedDerivedTypes);
        }
    }
}

void ContainerSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        if (symbol->IsExportSymbol())
        {
            if (collected.find(symbol.get()) == collected.end())
            {
                collected.insert(symbol.get());
                symbol->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
            }
        }
    }
}

void ContainerSymbol::InitVirtualFunctionTables()
{
    for (const std::unique_ptr<Symbol>& symbol : symbols)
    {
        symbol->InitVirtualFunctionTables();
    }
}

} } // namespace Cm::Sym
