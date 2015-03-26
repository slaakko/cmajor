/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ParameterSymbol.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <stdexcept>

namespace Cm { namespace Sym {

ParameterSymbol::ParameterSymbol(const Span& span_, const std::string& name_) : Symbol(span_, name_), type(nullptr)
{
}

void ParameterSymbol::Write(Writer& writer)
{
    Symbol::Write(writer);
    bool hasType = type != nullptr;
    writer.GetBinaryWriter().Write(hasType);
    if (!hasType) 
    {
        SymbolTable* symbolTable = writer.GetSymbolTable();
        Cm::Ast::Node* parameterNode = symbolTable->GetNode(this);
        if (!parameterNode)
        {
            throw std::runtime_error("write: parameter node not found from symbol table");
        }
        writer.GetAstWriter().Write(parameterNode);
    }
    else
    {
        writer.Write(type->Id());
    }
}

void ParameterSymbol::Read(Reader& reader)
{
    Symbol::Read(reader);
    bool hasType = reader.GetBinaryReader().ReadBool();
    if (hasType)
    {
        reader.FetchTypeFor(this, 0);
    }
    else
    {
        Cm::Ast::ParameterNode* parameterNode = reader.GetAstReader().ReadParameterNode();
        ownedParameterNode.reset(parameterNode);
        reader.GetSymbolTable().SetNode(this, parameterNode);
    }
}

TypeSymbol* ParameterSymbol::GetType() const
{
    return type;
}

void ParameterSymbol::SetType(TypeSymbol* type_, int index)
{
    type = type_;
}

void ParameterSymbol::CollectExportedDerivedTypes(std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    if (!type) return;
    if (type->IsDerivedTypeSymbol())
    {
        type->CollectExportedDerivedTypes(exportedDerivedTypes);
    }
}

void ParameterSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TemplateTypeSymbol*>& exportedTemplateTypes)
{
    if (!type) return;
    if (type->IsTemplateTypeSymbol() || type->IsDerivedTypeSymbol())
    {
        if (collected.find(type) == collected.end())
        {
            collected.insert(type);
            type->CollectExportedTemplateTypes(collected, exportedTemplateTypes);
        }
    }
}

} } // namespace Cm::Sym