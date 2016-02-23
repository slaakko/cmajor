/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/FunctionTemplateRepository.hpp>
#include <Cm.Sym/TypeSymbol.hpp>
#include <Cm.Sym/SymbolTable.hpp>
#include <fstream>

namespace Cm { namespace Core {

FunctionTemplateKey::FunctionTemplateKey(Cm::Sym::FunctionSymbol* subject_, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments_): subject(subject_), templateArguments(templateArguments_),
    hashCodeValid(false)
{
}

size_t FunctionTemplateKey::GetHashCode() const
{
    if (!hashCodeValid)
    {
        const_cast<FunctionTemplateKey*>(this)->ComputeHashCode();
    }
    return hashCode;
}

void FunctionTemplateKey::ComputeHashCode()
{
    hashCode = std::hash<Cm::Sym::FunctionSymbol*>()(subject);
    for (Cm::Sym::TypeSymbol* templateArgument : templateArguments)
    {
        size_t templateArgHashCode = templateArgument->Id().GetHashCode();
        hashCode ^= templateArgHashCode;
    }
    hashCodeValid = true;
}

bool operator==(const FunctionTemplateKey& left, const FunctionTemplateKey& right)
{
    return left.Subject() == right.Subject() && left.TemplateArguments() == right.TemplateArguments();
}

FunctionTemplateRepository::FunctionTemplateRepository(Cm::Sym::SymbolTable& symbolTable_) : symbolTable(symbolTable_)
{
}

FunctionTemplateRepository::~FunctionTemplateRepository()
{
}

Cm::Sym::FunctionSymbol* FunctionTemplateRepository::GetFunctionTemplateInstance(const FunctionTemplateKey& key) const
{
    FunctionTemplateMapIt i = functionTemplateMap.find(key);
    if (i != functionTemplateMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void FunctionTemplateRepository::AddFunctionTemplateInstance(const FunctionTemplateKey& key, Cm::Sym::FunctionSymbol* functionTemplateInstance)
{
    functionTemplateMap[key] = functionTemplateInstance;
}

void FunctionTemplateRepository::Write(Cm::Sym::BcuWriter& writer)
{
    int n = int(functionTemplateMap.size());
    writer.GetBinaryWriter().Write(n);
    FunctionTemplateMapIt e = functionTemplateMap.end();
    for (FunctionTemplateMapIt i = functionTemplateMap.begin(); i != e; ++i)
    {
        Cm::Sym::FunctionSymbol* functionSymbol = i->second;
        writer.GetBinaryWriter().Write(functionSymbol->Parent()->FullName());
        writer.GetSymbolWriter().Write(functionSymbol);
    }
}

void FunctionTemplateRepository::Read(Cm::Sym::BcuReader& reader)
{
    int n = reader.GetBinaryReader().ReadInt();
    for (int i = 0; i < n; ++i)
    {
        std::string parentName = reader.GetBinaryReader().ReadString();
        Cm::Sym::Symbol* parent = reader.GetSymbolReader().GetSymbolTable().GlobalScope()->Lookup(parentName);
        if (!parent)
        {
            throw std::runtime_error("got no parent");
        }
        Cm::Sym::Symbol* symbol = reader.GetSymbolReader().ReadSymbol();
        if (symbol->IsFunctionSymbol())
        {
            if (!symbol->Owned())
            {
                symbol->SetOwned();
                Cm::Sym::FunctionSymbol* functionSymbol = static_cast<Cm::Sym::FunctionSymbol*>(symbol);
                functionSymbol->SetParent(parent);
                functionSymbols.push_back(std::unique_ptr<Cm::Sym::FunctionSymbol>(functionSymbol));
            }
        }
        else
        {
            throw std::runtime_error("function symbol expected");
        }
    }
}

} } // namespace Cm::Core