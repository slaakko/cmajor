/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/ConceptSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Sym/Writer.hpp>
#include <Cm.Sym/Reader.hpp>

namespace Cm { namespace Sym {

ConceptSymbol::ConceptSymbol(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

void ConceptSymbol::Write(Writer& writer)
{
    ContainerSymbol::Write(writer);
    writer.Write(id);
    writer.GetBinaryWriter().Write(groupName);
    Cm::Ast::Node* node = writer.GetSymbolTable()->GetNode(this);
    if (node->IsConceptNode())
    {
        writer.GetAstWriter().Write(node);
    }
    else
    {
        throw std::runtime_error("write: concept node expected");
    }
}

void ConceptSymbol::Read(Reader& reader)
{
    ContainerSymbol::Read(reader);
    id = reader.ReadTypeId();
    groupName = reader.GetBinaryReader().ReadString();
    conceptNode.reset(reader.GetAstReader().ReadConceptNode());
    reader.GetSymbolTable().SetNode(this, conceptNode.get());
}

void ConceptSymbol::AddSymbol(Symbol* symbol)
{
    ContainerSymbol::AddSymbol(symbol);
    if (symbol->IsTypeParameterSymbol())
    {
        TypeParameterSymbol* typeParameterSymbol = static_cast<TypeParameterSymbol*>(symbol);
        typeParameterSymbol->SetIndex(int(typeParameters.size()));
        typeParameters.push_back(typeParameterSymbol);
    }
}

std::string MakeInstantiatedConceptSymbolName(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments)
{
    std::string s = conceptSymbol->GroupName();
    s.append(1, '<');
    bool first = true;
    for (TypeSymbol* typeArgument : typeArguments)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(typeArgument->FullName());
    }
    s.append(1, '>');
    return s;
}

TypeId ComputeInstantiatedConceptSymbolTypeId(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments)
{
    TypeId id = conceptSymbol->Id();
    for (TypeSymbol* typeArgument : typeArguments)
    {
        id.Rep() = id.Rep() ^ typeArgument->Id().Rep();
    }
    return id;
}

InstantiatedConceptSymbol::InstantiatedConceptSymbol(ConceptSymbol* conceptSymbol_, const std::vector<TypeSymbol*>& typeArguments_) :
    Symbol(conceptSymbol_->GetSpan(), MakeInstantiatedConceptSymbolName(conceptSymbol_, typeArguments_)), conceptSymbol(conceptSymbol_), typeArguments(typeArguments_)
{
}

} } // namespace Cm::Sym
