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
#include <Cm.Sym/TemplateTypeSymbol.hpp>
#include <stdexcept>

namespace Cm { namespace Sym {

ParameterSymbol::ParameterSymbol(const Span& span_, const std::string& name_) : VariableSymbol(span_, name_)
{
}

void ParameterSymbol::Write(Writer& writer)
{
    VariableSymbol::Write(writer);
    bool hasType = GetType() != nullptr;
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
}

void ParameterSymbol::Read(Reader& reader)
{
    VariableSymbol::Read(reader);
    bool hasType = HasType();
    if (!hasType)
    {
        Cm::Ast::ParameterNode* parameterNode = reader.GetAstReader().ReadParameterNode();
        ownedParameterNode.reset(parameterNode);
        reader.GetSymbolTable().SetNode(this, parameterNode);
    }
}

void ParameterSymbol::CollectExportedDerivedTypes(std::unordered_set<Symbol*>& collected, std::unordered_set<TypeSymbol*>& exportedDerivedTypes)
{
    TypeSymbol* type = GetType();
    if (!type) return;
    if (type->IsDerivedTypeSymbol())
    {
        if (collected.find(type) == collected.end())
        {
            collected.insert(type);
            type->CollectExportedDerivedTypes(collected, exportedDerivedTypes);
        }
    }
}

void ParameterSymbol::CollectExportedTemplateTypes(std::unordered_set<Symbol*>& collected, std::unordered_map<TypeId, TemplateTypeSymbol*, TypeIdHash>& exportedTemplateTypes)
{
    TypeSymbol* type = GetType();
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

void ParameterSymbol::ReplaceReplicaTypes()
{
    TypeSymbol* type = GetType();
    if (type->IsReplica() && type->IsTemplateTypeSymbol())
    {
        TemplateTypeSymbol* replica = static_cast<TemplateTypeSymbol*>(type);
        type = replica->GetPrimaryTemplateTypeSymbol();
    }
}

} } // namespace Cm::Sym
